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
#include "Layout.h"

#include <cpen333\process\mutex.h>
#include <cpen333\process\semaphore.h>
#include <cpen333\process\shared_memory.h>
#include <cpen333\thread\thread_object.h>

class Truck: public cpen333::thread::thread_object{
protected:
	int idx_;                     // truck ID
	double capacity = rand() % TRUCK_CAPACITY_VARIATION + TRUCK_CAPACITY - TRUCK_CAPACITY_VARIATION / 2;	// random capacity
	double currWeight = 0;

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
			memory_->tinfo.rtruckStatus[idx_] = TRUCK_DEPARTURE;	// signal computer about arrival
			memory_->tinfo.rcapcity[idx_] = capacity;
			memory_->tinfo.rweight[idx_] = currWeight;
		}
	}

	/**
	* @brief wait until semaphore notifies it's available
	* @return true if successful, false if not successful
	*/
	bool dock() {
		unloadingBay.wait();
		notifyArrival();
		return true;
	}

	/**
	* @brief change the status to ARRIVAL in shared memory
	*/
	void notifyArrival() {
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->tinfo.rtruckStatus[idx_] = TRUCK_ARRIVAL;	// signal computer about arrival
			memory_->tinfo.ndocked++;
		}
	}

	/**
	* @brief change the status to DEPARTURE in shared memory, notify sem
	*/
	void notifyLeaving() {
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->tinfo.rtruckStatus[idx_] = TRUCK_DEPARTURE;	// signal computer about arrival
			memory_->tinfo.ndocked--;
		}	
		unloadingBay.notify();
	}

	/**
	* truck will empty all items on truck, push into unloading queue until there is not items left
	*/
	void waitTillEmpty(Order& o) {
		std::pair<ItemInfo,int> itemInfo;
		while (o.getItemInfo(itemInfo)) {
			for (int i = 0; i < itemInfo.second; i++) {
				unloadingQueue.add(itemInfo.first);
				currWeight -= itemInfo.first.getWeight();
				updateCurrWeight();
				std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // hold on there for a bit
			}
		}
		unloadingQueue.add(ItemInfo(POISION_ID));		// signal robot for finish
	}

	/**
	* upload curre weight to shared memory
	*/
	void updateCurrWeight() {
		std::unique_lock<decltype(mutex_)> lock(mutex_);
		memory_->tinfo.rweight[idx_] = currWeight;
	}

	/**
	* Checks if we are supposed to quit
	* @return true if memory tells us to quit
	*/
	bool check_quit() {
		// check if we need to quit
		std::lock_guard<decltype(mutex_)> lock(mutex_);
		return memory_->quit;
	}

	/**
	* return capacity of truck
	*/
	double returnCapacity() {
		return capacity;
	}

	int main() {
		while (!check_quit()) {
			Order o;
			truckOrderQueue.getOrder(o);
			if (o.returnOrderID() == POISION_ID)
				break;
			else {
				// update current weight
				currWeight = o.getOrderWeight();
				updateCurrWeight();
				std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // hold on there for a bit

				if (dock()) {
					waitTillEmpty(o);
					std::this_thread::sleep_for(std::chrono::milliseconds(5000));
					notifyLeaving();
				}	
			}
		}
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
			memory_->tinfo.dtruckStatus[idx_] = TRUCK_DEPARTURE;	// signal computer about arrival
			memory_->tinfo.dcapcity[idx_] = capacity;
			memory_->tinfo.dweight[idx_] = currWeight;
		}
	}

	/**
	* @brief wait until semaphore notifies it's available
	* @return true if successful, false if not successful
	*/
	bool dock() {
		loadingBay.wait();
		notifyArrival();
		return true;
	}

	/**
	* @brief change the status to ARRIVAL in shared memory
	*/
	void notifyArrival() {
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->tinfo.dtruckStatus[idx_] = TRUCK_ARRIVAL;	// signal computer about arrival
			memory_->tinfo.ndocked++;
		}
	}

	/**
	* @brief change the status to DEPARTURE in shared memory, notify sem
	*/
	void notifyLeaving() {
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->tinfo.dtruckStatus[idx_] = TRUCK_DEPARTURE;	// signal computer about arrival
			memory_->tinfo.ndocked--;
		}
		loadingBay.notify();
	}

	/** 
	* truck will fill pop items from loading queue until both conditions are satisfied
	* 1. truck has finished one order
	* 2. truck has weight >80% of its capacity
	*/
	void waitTillFull() {
		bool finish = false;
		while (!check_quit() && !finish) {
			while (currWeight < capacity * FULL_RATIO || !finish) {
				ItemInfo i = loadingQueue.get();
				if (i.getID() == POISION_ID) {
					finish = true;
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(500)); // hold on there for a bit
				currWeight += i.getWeight();
				updateCurrWeight();
			}
		}
	}

	/**
	* upload curre weight to shared memory
	*/
	void updateCurrWeight() {
		std::unique_lock<decltype(mutex_)> lock(mutex_);
		memory_->tinfo.dweight[idx_] = currWeight;
	}

	/**
	* Checks if we are supposed to quit
	* @return true if memory tells us to quit
	*/
	bool check_quit() {
		// check if we need to quit
		std::lock_guard<decltype(mutex_)> lock(mutex_);
		return memory_->quit;
	}

	int main() {
		while (!check_quit()) {
			if (dock()) {
				waitTillFull();
				notifyLeaving();
			}
		}
		return 0;
	}

};
#endif //CPEN333_PROJECT_TRUCK_H
