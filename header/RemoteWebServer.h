//
// RemoteWebServer.h
//

#ifndef AMAZOOM_REMOTEWEBSERVER_H
#define AMAZOOM_REMOTEWEBSERVER_H

#define WAREHOUSE_SEM_NAME "msg2warehouse"
#define CLIENT_SEM_NAME "msg2client"
#define JSON_ID 255

#include <cpen333/process/mutex.h>
#include <cpen333/process/socket.h>
#include <mutex>
#include <string>
#include <iostream>
#include "Amazoom.h"
#include "Message.h"
#include <json.hpp>
#include <deque>
#include "jsonConverter.h"
#include "WarehouseComputerAPI.h"

using JSON = nlohmann::json;


class RemoteWebServer{
private:
    std::mutex mutex_;              // mutex for multi client connection
    std::deque<JSON> msg2warehouseQueue;
    std::deque<JSON> msg2clientQueue;
    cpen333::process::socket_server server;
    cpen333::process::socket_server whServer;


    /**
     * Main thread function for handling communication with a single remote
     * client. Message received from client socket is appended with client id
     * and  pushed onto msg2warehouseQueue.
     * Reads msg from msg2clientQueue, if the msg match the client ID,
     * pass the msg to client over client socket
     * @param api WarehouseComputerAPI for sending and recving messages from warehouseComputer
     * @param id client id
     */
    void serviceClient(WarehouseComputerAPI&& api, std::deque<JSON>& msg2warehouseQueue, std::deque<JSON>& msg2clientQueue, int id){
        // set up semaphores
        cpen333::process::semaphore whSem(WAREHOUSE_SEM_NAME, 0);
        cpen333::process::semaphore clientSem(CLIENT_SEM_NAME, 0);

        std::cout << "Client " << id << "is connected." << std::endl;

        // receive message
        std::unique_ptr<std::string> jsonStrPtr = api.recvJSON();

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

            // push to msg2warehouse queue
            {
                std::lock_guard<decltype(mutex_)> lock(mutex_);
                msg2warehouseQueue.push_back(std::move(j));
            } // for thread safety
            whSem.notify();

            // peek msg
            clientSem.wait();
            JSON& msg2client = msg2clientQueue.front();
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

        std::cout << "Warehouse " << id << "is connected" << std::endl;

        // pop front from msg2warehouse queue
        whSem.wait();
        JSON& msg2warehosue = msg2warehouseQueue.front();
        while (msg2warehosue!= nullptr){
            {
                std::lock_guard<decltype(mutex_)> lock(mutex_); // for thread safety
                msg2warehouseQueue.pop_front();
            }
            // remove front from queue

            // parse to string
            std::string jStr = msg2warehosue.dump();

            // send to warehouse
            api.sendJSON(jStr);

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
        cpen333::process::socket client;
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
        cpen333::process::socket whComputer;

        while(true){
            if(server.accept(client)) {
                WarehouseComputerAPI api(std::move(client));

                auto clientServiceThread = std::thread(this->serviceClient, std::move(api), std::ref(this->msg2warehouseQueue),
                                                       std::ref(this->msg2clientQueue), id);
                clientServiceThread.detach();
                id++;
            }
        }
    }
public:
    /**
     * Constructor, initializes socket server and warehouse server
     */
    RemoteWebServer(int client_port = CLIENT_PORT, int warehouse_port = WAREHOUSE_PORT):
            server(client_port), whServer(warehouse_port)  {
        msg2clientQueue = std::deque<JSON>();
        msg2warehouseQueue = std::deque<JSON>();
    }





    /**
     * listens for whComputer socket connection first
     * then listens for socket connection and put socket into separate thread
     */
    int main(){
        whServer.open();
        std::cout << "Warehouse Server Opens: " << whServer.port() << std::endl;
        server.open();
        std::cout << "Client server opens: " << server.open() << std::endl;

//        listenForWarehouse();
        auto whListener = std::thread(this->listenForWarehouse,std::move(whServer));
        whListener.detach();
        auto clientListener = std::thread(this->listenForClient, std::move(server));
        clientListener.detach();
    }

};

//class WarehouseComputerAPI{
//private:
//    cpen333::process::socket socket_;
//
//    bool readString(std::string& str, size_t size){
//        char cbuff[256];
//        size_t buffSize = sizeof(cbuff);
//
//        bool success = false;
//
//        int num2read = std::min(buffSize, size);
//        while (socket_.read_all(cbuff, num2read)){
//            for(int i=0; i<num2read; i++){
//                str += cbuff[i];
//            }
//
//            if (!success)
//                success = true;
//
//            if (size>buffSize)
//                size -= buffSize;
//            else
//                break;
//
//            num2read = std::min(buffSize, size);
//        }
//
//        return success;
//    }
//public:
//    /**
//     * Constructor for WarehouseComputerAPI
//     * @param client
//     */
//    WarehouseComputerAPI(cpen333::process::socket&& client) : socket_(std::move(client)){}
//
//    /**
//     * send JSON string message to warehouseComputer
//     */
//    void sendMessage(){}
//
//    /**
//     * send JSON string
//     * @param jsonStr
//     * @return
//     */
//    bool sendJSON(std::string jsonStr){
//        // encode json string size
//        char buff[4];
//        char jsonID = JSON_ID;
//        size_t size = jsonStr.size()+1;
//
//        for (int i=4; i-->0;){
//            // cut off byte and shift size over by 8 bits
//            buff[i] = (char)(size & 0xff);
//            size = size >> 8;
//        }
//
//        // write contents
//        bool success = socket_.write(&jsonID, 1);
//        if (success){
//            success &= socket_.write(buff, 4); // content size
//            success &= socket_.write(jsonStr); // content
//        }
//        return success;
//    }
//
//    /**
//     *
//     * @param j json object
//     */
//    bool sendJSON(const JSON& j){
//        std::string jsonStr = j.dump();
//
//        return sendJSON(jsonStr);
//    }
//
//    /**
//     *
//     * @return parsed message, nullptr if an error occurred
//     */
//    std::unique_ptr<Message> recvMessage(){}
//
//    /**
//     *
//     * @return unique pointer to JSON string, nullptr if error occurs
//     */
//    std::unique_ptr<std::string> recvJSON(){
//        char id;
//        char buff[4];
//        size_t size = 0;
//        if (socket_.read_all(&id, 1) || id != JSON_ID){
//            std::cout << "JSON ID does not match" << std::endl;
//            return nullptr;
//        }
//
//        // if if is a json string
//        if (!socket_.read_all(buff, 4)) {
//            std::cout << "Cannot read JSON size" << std::endl;
//            return nullptr;
//        }
//
//        for (int i = 0; i < 4; i++) {
//            char buff_char = buff[4 - 1 - i];// << 8 * i;
//            int temp = ((unsigned int)buff_char & 0xFF);
//            temp = temp << 8 * i;
//            size += temp;
//        }
//
//        // read entire JSON string
//        std::string str;
//        if (!readString(str, size)) {
//            return nullptr;
//        }
//
//        return std::unique_ptr<std::string>(new std::string(str));
//    }
//
//};

#endif //AMAZOOM_REMOTEWEBSERVER_H
