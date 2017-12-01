//
// Created by minghua on 30/11/17.
//

#ifndef AMAZOOM_REMOTEWEBSERVER_H
#define AMAZOOM_REMOTEWEBSERVER_H

#include <cpen333/process/mutex.h>
#include <cpen333/process/socket.h>
#include <mutex>
#include <string>


class RemoteServer{
private:
    std::mutex mutex_;              // mutex for multi client connection
    std::deque<std::string> msgQueue;
    cpen333::process::socket_server server;
    cpen333::process::socket client;
    cpen333::process::socket whComputer[WAREHOUSE_NUM];
    WarehouseComputerAPI api;
public:
    /**
     * Constructor, initializes socket server and warehouseComputer API
     */
    RemoteServer(){}

    /**
     * Main thread function for handling communication with a single remote
     * client. Message recved from client socket is pushed onto msgQueue.
     * @param api WarehouseComputerAPI for sending and recving messages from warehouseComputer
     * @param id client id
     */
    void serviceClient(int id){}

    /**
     * TO-DO: should there be two separate queue? one of a warehouse, and message is first pushed to queue#1, if not success from warehouse #1
     * the message is pushed to queue #2, then if warehouse #2 also returns a failure, this order is unsuccessful
     *
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
