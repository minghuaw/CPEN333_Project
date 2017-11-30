//
// Created by minghua on 30/11/17.
//

#ifndef CPEN333_PROJECT_TRUCK_H
#define CPEN333_PROJECT_TRUCK_H

#include <string>
#include <iostream>
#include <vector>
#include "Item.h"
#include "TruckQueue.h"
#include "Amazoom.h"

class Truck{
private:
    std::string ID;                     // truck ID
    double capacity;                    // loading capacity of the truck
    std::vector<Item> itemVector;       // vector storing all items on the truck
    TruckQueue truckQueue;              // communication queue with warehouseComputer
public:
    /**
     * @brief 
     * constructor for Truck class
     * @param truckID ID of the truck, std::string
     * @param cap capacity of the truck
     * @param items items that are on the truck
     * @param truckQ truckQ for notifying computer
     */
    Truck(std::string truckID, double cap, std::vector<Item> items, TruckQueue& truckQ){}
    /**
     * @brief wait until semaphore notifies it's available
     * @return true if successful, false if not successful
     */
    bool dock(){}

    /**
     * @brief notify computer about truck arrival using truckQueue
     * Cast ID, capacity, and action (arrive) into TruckInfo 
     * and push TruckInfo into TruckQueue
     */
    void notifyArrival(){}

    /**
     * @brief notify computer about truck leaving using truckQueue
     * Cast ID, capacity, and action (departure) into TruckInfo 
     * and push TruckInfo into TruckQueue
     */
    void notiryLeaving(){}
};
#endif //CPEN333_PROJECT_TRUCK_H
