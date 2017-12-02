//
// RestockingTruck.h
//

#ifndef AMAZOOM_RESTOCKTRUCK_H
#define AMAZOOM_RESTOCKTRUCK_H

#include "Truck.h"
#include "DynamicQueue.h"
#include "Order.h"

class RestockTruck: public Truck{
private:
    ItemQueue unloadingQueue;
    OrderQueue truckOrderQueue;
    Order order;
public:
    /**
     * @brief constructor, calls Truck's constructor
     * 
     * @param truckID ID of the truck, std::string
     * @param cap capacity of the truck
     * @param items items that are on the truck
     * @param truckQ truckQ for notifying computer
     * @param unloadQ ItemQ for robot to unload items
     * @param truckOrderQ order queue for restock truck to receive order from manager
     */
    RestockTruck(const std::string truckID, double cap, std::vector<Item> items,
                TruckQueue& truckQ, ItemQueue& unloadQ, OrderQueue& truckOrderQ) :Truck(truckID, cap, items, truckQ){}
    
    /**
     * @brief collect next restocking order from the truckOrderQueue
     * 
     * @param outorder reference to the Order object dequeued from truckOrderQueue
     */
    void collectOrder(Order& outorder){}

    /**
     * @brief wait until truck is empty
     * 
     */
    void waitTillEmpty(){}

    /**
     * @brief empty the items on the truck
     * 
     */
    void empty(){}

};

#endif //AMAZOOM_RESTOCKTRUCK_H
