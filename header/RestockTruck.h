//
// Created by minghua on 30/11/17.
//

#ifndef AMAZOOM_RESTOCKTRUCK_H
#define AMAZOOM_RESTOCKTRUCK_H

#include "Truck.h"
#include "ItemQueue.h"
#include "OrderQueue.h"
#include "Order.h"

class RestockTruck: public Truck{
private:
    ItemQueue unloadingQueue;
    OrderQueue truckOrderQueue;
    Order& order;
public:
    void

}

#endif //AMAZOOM_RESTOCKTRUCK_H
