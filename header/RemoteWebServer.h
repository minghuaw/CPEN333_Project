//
// Created by minghua on 30/11/17.
//

#ifndef AMAZOOM_REMOTEWEBSERVER_H
#define AMAZOOM_REMOTEWEBSERVER_H

#include <cpen333/process/mutex.h>
#include <cpen333/process/socket.h>
#include <mutex>
#include <string>


class RemoteWebServer{
private:
    std::mutex mutex_;              // mutex for multi client connection
    std::deque<std::string> msg2warehouseQueue;
    std::deque<std::string> msg2clientQueue;
    cpen333::process::socket_server server;
    cpen333::process::socket client;
    cpen333::process::socket whComputer[WAREHOUSE_NUM];
    WarehouseComputerAPI api;
public:
    /**
     * Constructor, initializes socket server and warehouseComputer API
     */
    RemoteWebServer(){}

    /**
     * Main thread function for handling communication with a single remote
     * client. Message received from client socket is appended with client id
     * and  pushed onto msg2warehouseQueue.
     * Reads msg from msg2clientQueue, if the msg match the client ID,
     * pass the msg to client over client socket
     * @param api WarehouseComputerAPI for sending and recving messages from warehouseComputer
     * @param id client id
     */
    void serviceClient(int id){}

    /**
     * Main thread function for handling communication with a single remote warehouse computer
     * Message on the msgQueue is popped and sent to warehouseComputer on whComputer sockets
     * @param api warehouseComputreAPI
     * @param id warehouse ID
     */
    void serviceWarehouse(WarehouseComputerAPI& api, int id){}
};

class WarehouseComputerAPI{
private:
    cpen333::process::socket client;
public:
    /**
     * Constructor for WarehouseComputerAPI
     * @param client
     */
    WarehouseComputerAPI(cpen333::process::socket&& client){}

    /**
     * send JSON string message to warehouseComputer
     */
    void sendMessage(){}

    /**
     * recv JSON string message from warehouseComputer
     */
    void recvMessage(){}

};

#endif //AMAZOOM_REMOTEWEBSERVER_H
