//
// 
//

#ifndef AMAZOOM_DELIVERYTRUCK_H
#define AMAZOOM_DELIVERYTRUCK_H

#include <string>
#include "Truck.h" 
#include "ItemQueue.h"
#include "Item.h"

class DeliveryTruck: public Truck{
private:
    ItemQueue loadingQueue;
public:
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
}
#endif //AMAZOOM_DELIVERYTRUCK_H
