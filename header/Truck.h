//
// Truck.h
//

#ifndef CPEN333_PROJECT_TRUCK_H
#define CPEN333_PROJECT_TRUCK_H

#include <string>
#include <iostream>
#include <vector>
#include "Item.h"
#include "DynamicQueue.h"
#include "Amazoom.h"

#include<cpen333\process\semaphore.h>

class Truck: public cpen333::thread::thread_object{
protected:
	int idx_;                     // truck ID
	double capacity;                    // loading capacity of the truck
	
public:
	/**
	* default constructor
	*/
	Truck(){};

    /**
     * @brief wait until semaphore notifies it's available
     * @return true if successful, false if not successful
     */
	virtual bool dock() { return true; }

    /**
     * @brief change the status to ARRIVAL in shared memory
     */
    virtual void notifyArrival(){}

    /**
     * @brief change the status to DEPARTURE in shared memory
     */
    virtual void notifyLeaving(){}
};

class RestockTruck : public Truck {
private:
	// shared information, shared among LayoutGUI and Warehouse processes
	cpen333::process::shared_object<SharedData> memory_;
	cpen333::process::mutex mutex_;
	// unloading bay
	cpen333::process::semaphore unloadingBay;

	ItemQueue& unloadingQueue;
	OrderQueue& truckOrderQueue;
	
	std::vector<Item> itemVector;       // vector storing all items on the truck

	//Order& order; // TODO: ???? why is this here
	//TruckQueue& truckQueue;              // communication queue with warehouseComputer

public:
	/**
	* @brief
	* constructor for RestockTruck class
	* @param truckID ID of the truck, std::string
	* @param cap capacity of the truck
	* @param items items that are on the truck
	* @param truckQ truckQ for notifying computer
	*/
	RestockTruck(OrderQueue& truckOrderQ, ItemQueue& unloadingQ): \
		unloadingBay(UNLOADING_BAY_NAME, LOADING_BAY_SEM_RESOURCE),\
		unloadingQueue(unloadingQ), truckOrderQueue(truckOrderQ),\
		memory_(WAREHOUSE_MEMORY_NAME), mutex_(WAREHOUSE_MEMORY_MUTEX_NAME)	
	{
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			idx_ = memory_->tinfo.nrtrucks;
			memory_->tinfo.nrtrucks++;
			memory_->tinfo.rtruckStatus[idx_] = TRUCK_ARRIVAL;	// signal computer about arrival
		}
		capacity = rand() % 30 + TRUCK_CAPACITY - 15;	// random capacity, range in 435-449
	}

	/**
	* @brief wait until semaphore notifies it's available
	* @return true if successful, false if not successful
	*/
	bool dock() {
		unloadingBay.wait();
		return true;
	}

	/**
	* @brief change the status to ARRIVAL in shared memory
	*/
	void notifyArrival() {
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->tinfo.rtruckStatus[idx_] = TRUCK_ARRIVAL;	// signal computer about arrival
		}
	}

	/**
	* @brief change the status to DEPARTURE in shared memory, notify sem
	*/
	void notifyLeaving() {
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->tinfo.rtruckStatus[idx_] = TRUCK_DEPARTURE;	// signal computer about arrival
		}	
		unloadingBay.notify();
	}

	/**
	* collect order, constantly called by int main in a loop, try to dequeue truckOrderQueue 
	* after getting an order, also generate LIST OF ITEMS
	* @param outOrder	order to output
	*/
	void collectOrder(Order& outOrder) {

	}

	/**
	* truck will empty all items on truck, push into unloading queue until there is not items left
	* continouously call empty() in a loop
	*/
	void waitTillEmpty() {

	}

	/**
	* truck will empty one item to unloading queue
	*/
	void empty() {

	}

	int main() {
		std::cout << " I am a restock truck" << std::endl;
		return 0;
	}
};

class DeliveryTruck : public Truck {
private:
	// shared information, shared among LayoutGUI and Warehouse processes
	cpen333::process::shared_object<SharedData> memory_;
	cpen333::process::mutex mutex_;
	// loading bay
	cpen333::process::semaphore loadingBay;

	ItemQueue& loadingQueue;

	std::vector<Item> itemVector;       // vector storing all items on the truck

	//TruckQueue& truckQueue;             // communication queue with warehouseComputer
	//Order& order;						// used to verify if an order has been completed

public:
	/**
	* @brief
	* constructor for RestockTruck class
	* @param truckID ID of the truck, std::string
	* @param cap capacity of the truck
	* @param items items that are on the truck
	* @param truckQ truckQ for notifying computer
	*/
	DeliveryTruck(ItemQueue& loadingQ) : \
		loadingQueue(loadingQ),\
		loadingBay(LOADING_BAY_NAME, LOADING_BAY_SEM_RESOURCE), \
		memory_(WAREHOUSE_MEMORY_NAME), mutex_(WAREHOUSE_MEMORY_MUTEX_NAME)
	{
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			idx_ = memory_->tinfo.ndtrucks;
			memory_->tinfo.ndtrucks++;
			memory_->tinfo.dtruckStatus[idx_] = TRUCK_ARRIVAL;	// signal computer about arrival
		}
		capacity = rand() % 30 + TRUCK_CAPACITY-15;	// random capacity, range in 435-449
	}

	/**
	* @brief wait until semaphore notifies it's available
	* @return true if successful, false if not successful
	*/
	bool dock() {
		loadingBay.wait();
		return true;
	}

	/**
	* @brief change the status to ARRIVAL in shared memory
	*/
	void notifyArrival() {
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->tinfo.dtruckStatus[idx_] = TRUCK_ARRIVAL;	// signal computer about arrival
		}
	}

	/**
	* @brief change the status to DEPARTURE in shared memory, notify sem
	*/
	void notifyLeaving() {
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->tinfo.dtruckStatus[idx_] = TRUCK_DEPARTURE;	// signal computer about arrival
		}
		loadingBay.notify();
	}

	/** 
	* truck will fill pop items from loading queue until both conditions are satisfied
	* 1. truck has finished one order
	* 2. truck has weight >80% of its capacity
	*/
	void waitTillFull() {

	}

	int main() {
		std::cout << "I am a truck" << std::endl;
		notifyLeaving();
		return 0;
	}

};
#endif //CPEN333_PROJECT_TRUCK_H
