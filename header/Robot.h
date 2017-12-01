//
// Created by mli on 11/30/2017.
//

#ifndef AMAZOOM_ROBOT_H
#define AMAZOOM_ROBOT_H

#include "Order.h"
#include "Item.h"
#include "DynamicQueue.h"
#include "InventoryDatabase.h"
#include "Layout.h"

#define HOME_COL = A
#define HOME_ROW = 1

class Robot {
private:
    std::string ID;
    char col;
    int row;
    Order& order; // TODO: maybe wrong? should it be order?
    double capacity;
    std::vector<std::pair<Item,int>> items;
    double currWeight;
    ItemQueue unloadingQueue;
    OrderQueue robotOrderQueue;
    std::mutex db_mutex;    // for database protection
    InventoryDatabase& database;
    Layout warehouseLayout;

public:
    Robot(InventoryDatabase& database, std::string ID): database(database),ID(ID) {}
    collectOrder(Order& outorder){}
    pickItemFromShelf(){}
    pickItemFromUnloading(){}
    putItemOnShelf(){}
    putItemOnLoading(){}
    setLocation(){}
    /**
     *
     * @param col
     * @param row
     * @return
     */
    go(char col,int row){}
    returnHome(){}
    updateOrderProcessing(){}
    updateOrderFinished(){}
};


#endif //AMAZOOM_ROBOT_H
