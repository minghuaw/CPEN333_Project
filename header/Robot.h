//
// Robot.h
//

#ifndef AMAZOOM_ROBOT_H
#define AMAZOOM_ROBOT_H

#include "Order.h"
#include "Item.h"
#include "DynamicQueue.h"
#include "InventoryDatabase.h"
#include "Layout.h"
#include <cpen333\process\shared_memory.h>
#include <cpen333\process\mutex.h>
#include <cpen333/thread/thread_object.h>
#include "safe_printf.h"

class Robot: public cpen333::thread::thread_object {
private:
	// shared information layout info and robot info, shared among LayoutGUI and Warehouse processes
	cpen333::process::shared_object<SharedData> memory_;
	cpen333::process::mutex mutex_;

    //Order& order;                       // TODO: maybe wrong? should it be order?

    double capacity = ROBOT_CAPACITY;
	double currWeight = 0;

    std::vector<ItemInfo> items;
    
    ItemQueue& unloadingQueue;
	ItemQueue& loadingQueue;

    OrderQueue& robotOrderQueue;
    cpen333::process::mutex db_mutex;       // for database protection
    InventoryDatabase& database;			// shared database
	LayoutInfo linfo_;             // local copy of layout

	int loc_[2];   // current location
	Robot();	// prevent default constructor

public:
	int idx_;

    Robot(InventoryDatabase& database, OrderQueue& q, ItemQueue& unloadingQ, ItemQueue& loadingQ): \
		database(database),db_mutex(DB_MUTEX_NAME),\
		mutex_(WAREHOUSE_MEMORY_MUTEX_NAME),memory_(WAREHOUSE_MEMORY_NAME),\
		robotOrderQueue(q),unloadingQueue(unloadingQ), loadingQueue(loadingQ){
		{
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			linfo_ = memory_->linfo;
			idx_ = memory_->rinfo.nrobots;
			memory_->rinfo.nrobots++;

			// get current location
			loc_[COL_IDX] = memory_->rinfo.rloc[idx_][COL_IDX];
			loc_[ROW_IDX] = memory_->rinfo.rloc[idx_][ROW_IDX];
		}
	}

    /**
     *  collect order from robotOrderQueue, update internal Order& order
     */
    void collectOrder(){}

    /**
     * picks item on cell, add pairs in items, call cell "removeItem"
     */
    void pickItemFromShelf(){}

    /**
     * picks item on loading bay, add pairs in items
     * a poison pill will be given by restocking truck
     * to indicate an end of itemQueue
     */
    void pickItemFromUnloading(){}

    /**
     * put item on cell, remove pairs in items, call cell "addItem"
     */
    void putItemOnShelf(){}

    /**
     * put item on loadingQueue, remove pairs in items
     */
    void putItemOnLoading(){}

    /**
     * set the location of robot
     * @param col   the column of location to be set to robot
     * @param row   the row of location to be set to robot
     */
    void setLocation(char col, int row){}

    /**
     * let the robot recursively go the specified location at col, row
     * @param col   the column of location
     * @param row   the row of location
     * @return true if the position has been reached
     */
    bool go(int col,int row){
		// current location
		int c = loc_[COL_IDX];
		int r = loc_[ROW_IDX];

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		{
			// update position
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			// check for quit
			if (memory_->quit) {
				return -1;
			}
			memory_->rinfo.rloc[idx_][COL_IDX] = c;
			memory_->rinfo.rloc[idx_][ROW_IDX] = r;
		}

		if (c == col && r == row) {
			return 1;
		}

		// temporarily replace currently location with wall character to block it from going back
		linfo_.layout[c][r] = WALL_CHAR;

		// recursive search
		char curr_c = linfo_.layout[c - 1][r];
		if (curr_c != WALL_CHAR && curr_c != SHELF_CHAR && curr_c != HOME_CHAR && curr_c != LOADINGBAY_CHAR && curr_c != UNLOADINGBAY_CHAR) {
			loc_[COL_IDX] = c - 1;
			loc_[ROW_IDX] = r;
			int success = go(col, row);
			if (success != 0) {
				return success;
			}
		}
		curr_c = linfo_.layout[c][r - 1];
		if (curr_c != WALL_CHAR && curr_c != SHELF_CHAR && curr_c != HOME_CHAR && curr_c != LOADINGBAY_CHAR && curr_c != UNLOADINGBAY_CHAR) {
			loc_[COL_IDX] = c;
			loc_[ROW_IDX] = r - 1;
			int success = go(col,row);
			if (success != 0) {
				return success;
			}
		}
		curr_c = linfo_.layout[c + 1][r];
		if (curr_c != WALL_CHAR && curr_c != SHELF_CHAR && curr_c != HOME_CHAR && curr_c != LOADINGBAY_CHAR && curr_c != UNLOADINGBAY_CHAR) {
			loc_[COL_IDX] = c + 1;
			loc_[ROW_IDX] = r;
			int success = go(col,row);
			if (success != 0) {
				return success;
			}
		}
		curr_c = linfo_.layout[c][r + 1];
		if (curr_c != WALL_CHAR && curr_c != SHELF_CHAR && curr_c != HOME_CHAR && curr_c != LOADINGBAY_CHAR && curr_c != UNLOADINGBAY_CHAR) {
			loc_[COL_IDX] = c;
			loc_[ROW_IDX] = r + 1;
			int success = go(col,row);
			if (success != 0) {
				linfo_ = memory_->linfo;		// renew the local copy to get rid off the virutal walls
				return success;
			}
		}

		// failed, update the position and roll back
		loc_[COL_IDX] = c;
		loc_[ROW_IDX] = r;
		linfo_.layout[c][r] = EMPTY_CHAR;
		{
			// update position
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->rinfo.rloc[idx_][COL_IDX] = c;
			memory_->rinfo.rloc[idx_][ROW_IDX] = r;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// failed to find exit
		return 0;
	}

	/**
	* let the robot go to coordinate, call go
	* convert letter column to int
	* @param coor	coordinate of item
	*/
	void goToCoordinate(Coordinate coor) {
		char col_c = coor.getCol();
		int col = col_c - 'A';
		int row = coor.getRow();
		char side = coor.getSide();
		if (side == 'L')
			col--;
		else
			col++;
		go(col, row);
	}

    /**
     * robot returns to the home location
     */
    void returnHome(){}

    /**
     * update the order that the robot currently has to processing, once robot picks one item on shelf
     */
    void updateOrderProcessing(){}

    /**
     * update the order that the robot currently has to finished, after robot has put all order items on loadingQueue
     */
    void updateOrderFinished(){}

	/**
	* set position to -1,-1 to signal layout to remove me
	*/
	void removeMe() {
		loc_[COL_IDX] = -1;
		loc_[ROW_IDX] = -1;
		{
			// update position
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			memory_->rinfo.rloc[idx_][COL_IDX] = loc_[COL_IDX];
			memory_->rinfo.rloc[idx_][ROW_IDX] = loc_[ROW_IDX];
		}
	}

	/**
	* let robot go home
	*/
	void homeRobot() {
		if (go(linfo_.home[COL_IDX], linfo_.home[ROW_IDX] - 1)) {
			loc_[COL_IDX] = linfo_.home[COL_IDX];
			loc_[ROW_IDX] = linfo_.home[ROW_IDX];
			{
				// update position
				std::unique_lock<decltype(mutex_)> lock(mutex_);
				memory_->rinfo.rloc[idx_][COL_IDX] = loc_[COL_IDX];
				memory_->rinfo.rloc[idx_][ROW_IDX] = loc_[ROW_IDX];
			}
		}

	}

	/**
	* parse the iteminfo in an order, call go continously
	*/
	void process_order(Order& o) {
		// change order status
		robotOrderQueue.updateOrderStatus(o.returnOrderID(), OrderStatus::PROCESSING);

		// if the order is from manager
		if (o.returnOrderType() == OrderType::MANAGER) {
			process_manager_order(o);
		}
		else {
			process_client_order(o);
		}

		// change order status
		robotOrderQueue.updateOrderStatus(o.returnOrderID(), OrderStatus::SHIPPED);

	}

	/**
	* process manager order, first go to unloading bay, load items and update currWeight, 
	* if exceeding capacitance, unload and then comeback
	*/
	void process_manager_order(Order& o){
		// continously get iteminfo from queue
		bool finish = false;
		while (!finish){
			// go to unloading bay
			if (go(linfo_.unloading[COL_IDX], linfo_.unloading[ROW_IDX] - 1)) {
				// load until capacity is met
				while (currWeight < capacity) {
					ItemInfo i = unloadingQueue.get();
					if (i.getID() == POISION_ID) {
						finish = true;
						break;
					}	
					std::this_thread::sleep_for(std::chrono::milliseconds(500)); // hold on there for a bit
					currWeight += i.getWeight();
					items.push_back(i);
				}
				// unload each item
				for (auto& i : items) {
					goToCoordinate(i.getLocation());
					std::this_thread::sleep_for(std::chrono::milliseconds(500)); // hold on there for a bit
					database.addItemInfo(i);
				}
				// clear items after unloading
				items.clear();
				currWeight = 0;
			}
		}
		homeRobot();
	}

	/**
	* process client order, first load items and update currWeight,
	* if exceeding capacitance, go to unloading bay, unload and then comeback
	*/
	void process_client_order(Order& o){
		//std::cout << "client order" << std::endl;
		std::pair<ItemInfo, int> itemInfo;
		bool finish = false;
		while (!finish) {
			if (o.getItemInfo(itemInfo)) {
				// add item one by one
				int quantity = itemInfo.second, i = 0;
				ItemInfo item = itemInfo.first;
				while (i < quantity) {
					// load until capacity is met
					if (currWeight < capacity) {
						items.push_back(item);
						currWeight += item.getWeight();
						Coordinate coor = database.findItemLocation(item.getID());
						goToCoordinate(coor);
						std::this_thread::sleep_for(std::chrono::milliseconds(500)); // hold on there for a bit
						database.reduceItemInfoQuantity(item, 1);
						i++;
					}
					// robot full, go to loading bay
					else if (go(linfo_.loading[COL_IDX], linfo_.loading[ROW_IDX] - 1)) {
						std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // hold on there for a bit
						// load items to loading bay
						for (auto& i : items) {
							loadingQueue.add(i);
						}
						// clear items
						items.clear();
						currWeight = 0;
					}
				}
			}
			else {
				finish = true;
			}
		}
		loadingQueue.add(ItemInfo(POISION_ID));		// signal truck for finish
		homeRobot();
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
			Order o;
			robotOrderQueue.getOrder(std::ref(o));
			if (o.returnOrderID() == POISION_ID)
				break;
			else {
				process_order(std::ref(o));
				homeRobot();
			}
		}
		removeMe();
		return 0;
	}
};


#endif //AMAZOOM_ROBOT_H
