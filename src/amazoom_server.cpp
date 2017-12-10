//
// Created by minghua on 08/12/17.
//

#define WAREHOUSE_SEM_NAME "msg2warehouse"
#define CLIENT_SEM_NAME "msg2client"
#define JSON_ID 255

#include <cpen333/process/mutex.h>
#include <cpen333/process/socket.h>
#include <cpen333/process/semaphore.h>
#include <mutex>
#include <string>
#include <iostream>
#include "Amazoom.h"
#include "Message.h"
#include <json.hpp>
#include <deque>
#include "JsonConverter.h"
#include "WarehouseComputerAPI.h"

using JSON = nlohmann::json;

std::mutex mutex_;              // mutex for multi client connection
std::deque<JSON> msg2warehouseQueue;
std::deque<JSON> msg2clientQueue;
cpen333::process::socket_server server(CLIENT_PORT);
cpen333::process::socket_server whServer(WAREHOUSE_PORT);

void serviceClient(WarehouseComputerAPI&& api, std::deque<JSON>& msg2warehouseQueue, std::deque<JSON>& msg2clientQueue, int id){
    // set up semaphores
    cpen333::process::semaphore whSem(WAREHOUSE_SEM_NAME, 0);
    cpen333::process::semaphore clientSem(CLIENT_SEM_NAME, 0);

    std::cout << "Client " << id << " is connected." << std::endl;

    // receive message
    std::unique_ptr<std::string> jsonStrPtr = api.recvJSON();
	std::cout << "[server]" << *jsonStrPtr << std::endl; // test
    while(jsonStrPtr!= nullptr){
        // convert to JSON object and append client ID
        JSON j;
        try{
            j = JSON::parse(*jsonStrPtr);
            j[MESSAGE_CLIENT_ID] = id;
        }
        catch (JSON::exception& e){
            std::cerr << e.what();
        }

		std::cout << "[CLIENT ID]" << j.dump() << std::endl;

        // push to msg2warehouse queue
        {
            std::lock_guard<decltype(mutex_)> lock(mutex_);
            msg2warehouseQueue.push_back(std::move(j));
        } // for thread safety

		std::cout << "append to msg2warehouse queue" << std::endl;
        whSem.notify();

        // peek msg
        clientSem.wait();
        JSON& msg2client = msg2clientQueue.front();
		std::cout << "pick from msg2client queue" << std::endl;
        int clientID = msg2client[MESSAGE_CLIENT_ID];
        while (clientID != id){
            clientSem.notify(); // notify other client thread if the message is for another client

            std::this_thread::sleep_for(std::chrono::seconds(1)); // sleep one second to allow other thread retrieve the msg
            msg2client = msg2clientQueue.front();
            clientID = msg2client[MESSAGE_CLIENT_ID];
        }
        {
            std::lock_guard<decltype(mutex_)> lock(mutex_); // thread safety
            msg2clientQueue.pop_front();
        }
        JSON jStr = msg2client.dump();
        api.sendJSON(jStr);

        // receive message
        jsonStrPtr = api.recvJSON();
    }
}



/**
 * Main thread function for handling communication with a single remote warehouse computer
 * Message on the msgQueue is popped and sent to warehouseComputer on whComputer sockets
 * @param api && warehouse computer api
 * @param msg2warehouseQueue & msg queue for msg to warehouse
 * @param msg2clientQueue & msg queue for msg to client
 * @param id client id
 */
void serviceWarehouse(WarehouseComputerAPI&& api, std::deque<JSON>& msg2warehouseQueue, std::deque<JSON>& msg2clientQueue, int id){
    // set up semaphores
    cpen333::process::semaphore whSem(WAREHOUSE_SEM_NAME, 0);
    cpen333::process::semaphore clientSem(CLIENT_SEM_NAME, 0);

    std::cout << "Warehouse " << id << " is connected" << std::endl;

    // pop front from msg2warehouse queue
    whSem.wait();
    JSON msg2warehosue = msg2warehouseQueue.front();
    while (msg2warehosue!= nullptr){
        {
            std::lock_guard<decltype(mutex_)> lock(mutex_); // for thread safety
            msg2warehouseQueue.pop_front();
        }
        // remove front from queue

        // parse to string
        std::string jStr = msg2warehosue.dump();

		// test
		std::cout << jStr << std::endl;

        // send to warehouse
        api.sendJSON(jStr);
		
		// test
		std::cout << "Sending to warehouse" << std::endl;

        // receive response message from warehouse computer
        std::unique_ptr<std::string> jsonStrPtr = api.recvJSON();

        // parse string to JSON object
        JSON msg2client;
        try{
            msg2client = JSON::parse(*jsonStrPtr);
        }
        catch (JSON::exception& e){
            std::cerr << e.what();
        }

        // append to msg2client queue and notify client threads
        {
            std::lock_guard<decltype(mutex_)> lock(mutex_); // for thread safety
            msg2clientQueue.push_back(msg2client);
        }
        clientSem.notify();

        // pop front from msg2warehouse queue
        whSem.wait();
        {
            std::lock_guard<decltype(mutex_)> lock(mutex_); // for thread safety
            msg2warehosue = msg2warehouseQueue.front();
        }
    }
}

/**
 * listens for socket connection from warehouse computer
 * @param whServer warehouse server, already open
 */
void listenForWarehouse(){
    whServer.open();
    int id = 0;
    //cpen333::process::socket client;
    cpen333::process::socket whComputer;

    while(true){
        if(whServer.accept(whComputer)){
            WarehouseComputerAPI api(std::move(whComputer));

            auto whServiceThread = std::thread(serviceWarehouse, std::move(api), std::ref(msg2warehouseQueue),
                                               std::ref(msg2clientQueue),  id);
            whServiceThread.detach();
            id++;
        }
    }

}

/**
 * listens for socket connection from a client
 * @param server client server, already open
 */
void listenForClient(){
    int id = 0;
    cpen333::process::socket client;
    //cpen333::process::socket whComputer;

    while(true){
        if(server.accept(client)) {
            WarehouseComputerAPI api(std::move(client));

            auto clientServiceThread = std::thread(serviceClient, std::move(api), std::ref(msg2warehouseQueue),
                                                   std::ref(msg2clientQueue), id);
            clientServiceThread.detach();
            id++;
        }
    }
}

int main(){
    whServer.open();
    std::cout << "Warehouse Server Opens: " << whServer.port() << std::endl;
    server.open();
    std::cout << "Client server opens: " << server.port() << std::endl;

//        listenForWarehouse();
    auto whListener = std::thread(listenForWarehouse);
//    whListener.detach();
    auto clientListener = std::thread(listenForClient);
//    clientListener.detach();
    whListener.join();
    clientListener.join();
}