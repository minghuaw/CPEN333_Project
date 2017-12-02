//
// Created by mli on 11/30/2017.
//

#ifndef AMAZOOM_ROBOT_H
#define AMAZOOM_ROBOT_H

#include "Order.h"
#include "Item.h"
#include "DynamicQueue.h"
#include "InventoryDatabase.h"
#include "Layout_archived.h"

class Robot {
private:
    cpen333::process::shared_object<SharedData> memory_;    // warehouse info
    cpen333::process::mutex mutex_;

    std::string ID;
    char col;
    int row;
    Order& order;                       // TODO: maybe wrong? should it be order?
    double capacity;
    std::vector<std::pair<Item,int>> items;
    double currWeight;
    ItemQueue unloadingQueue;
    OrderQueue robotOrderQueue;
    std::mutex db_mutex;                // for database protection
    InventoryDatabase& database;        // shared database
    Layout warehouseLayout;             // local copy of layout


public:
    Robot(InventoryDatabase& database, std::string ID): database(database),ID(ID) {}

    /**
     *  collect order from robotOrderQueue, update internal Order& order
     */
    void collectOrder(){}

    /**
     * picks item on cell, add pairs in items, call cell "removeItem"
     */
    pickItemFromShelf(){}

    /**
     * picks item on loading bay, add pairs in items
     */
    pickItemFromUnloading(){}

    /**
     * put item on cell, remove pairs in items, call cell "addItem"
     */
    putItemOnShelf(){}

    /**
     * put item on loadingQueue, remove pairs in items
     */
    putItemOnLoading(){}

    /**
     * set the location of robot
     * @param col   the column of location to be set to robot
     * @param row   the row of location to be set to robot
     */
    setLocation(char col, int row){}

    /**
     * let the robot recursively go the specified location at col, row
     * @param col   the column of location
     * @param row   the row of location
     * @return true if the position has been reached
     */
    bool go(char col,int row){}

    /**
     * robot returns to the home location
     */
    returnHome(){}

    /**
     * update the order that the robot currently has to processing, once robot picks one item on shelf
     */
    updateOrderProcessing(){}

    /**
     * update the order that the robot currently has to finished, after robot has put all order items on loadingQueue
     */
    updateOrderFinished(){}
};


#endif //AMAZOOM_ROBOT_H
