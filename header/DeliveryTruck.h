//
// 
//

#ifndef AMAZOOM_DELIVERYTRUCK_H
#define AMAZOOM_DELIVERYTRUCK_H

#include <string>
#include <vector>
#include "Amazoom.h"
#include "Truck.h" 
#include "ItemQueue.h"
#include "Item.h"

class DeliveryTruck: public Truck{
private:
    ItemQueue loadingQueue;
public:
    /**
     * @brief constructor, calls Truck's constructor
     * 
     * @param truckID ID of the truck, std::string
     * @param cap capacity of the truck
     * @param items items that are on the truck
     * @param truckQ truckQ for notifying computer
     * @param loadQ ItemQ for robot to load items
     */
    DeliveryTruck(std::string truckID, double cap, std::vector<Item> items, TruckQueue& truckQ, ItemQueue& loadQ) :SuperClass(truckID, cap, items, truckQ){}

    /**
     * @brief wait at the loading bay until both
     * 1. truck is full enough
     * 2. all orders are completely loaded
     */
    void waitTillFull(){}

    /**
     * @brief taking items from loadingQueue
     *  
     */
    void fill(){}
};
#endif //AMAZOOM_DELIVERYTRUCK_H
