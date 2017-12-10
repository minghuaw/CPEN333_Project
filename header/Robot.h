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
	cpen333::process::mutex ly_mutex_;
    
    char col;
    int row;
    //Order& order;                       // TODO: maybe wrong? should it be order?
    double capacity;
    std::vector<std::pair<Item,int>> items;
    double currWeight;
    ItemQueue unloadingQueue;
    OrderQueue robotOrderQueue;
    cpen333::process::mutex db_mutex;       // for database protection
    InventoryDatabase& database;			// shared database
	LayoutInfo linfo_;             // local copy of layout

	int loc_[2];   // current location
	Robot();	// prevent default constructor

	DynamicQueue<int>& robotPosQueue;

public:
	int idx_;

    Robot(InventoryDatabase& database, DynamicQueue<int>& q): \
		database(database),db_mutex(DB_MUTEX_NAME),\
		ly_mutex_(LAYOUT_MEMORY_MUTEX_NAME),memory_(LAYOUT_MEMORY_NAME),\
		robotPosQueue(q){
		{
			std::unique_lock<decltype(ly_mutex_)> lock(ly_mutex_);
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
			std::unique_lock<decltype(ly_mutex_)> lock(ly_mutex_);
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
				return success;
			}
		}

		// failed, update the position and roll back
		loc_[COL_IDX] = c;
		loc_[ROW_IDX] = r;
		linfo_.layout[c][r] = EMPTY_CHAR;
		{
			// update position
			std::unique_lock<decltype(ly_mutex_)> lock(ly_mutex_);
			memory_->rinfo.rloc[idx_][COL_IDX] = c;
			memory_->rinfo.rloc[idx_][ROW_IDX] = r;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// failed to find exit
		return 0;
	}

	/**
	* let the robot go to coordinate, call go
	* @param coor	coordinate of item
	*/
	void goToCoordinate(Coordinate coor) {}

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
			std::unique_lock<decltype(ly_mutex_)> lock(ly_mutex_);
			memory_->rinfo.rloc[idx_][COL_IDX] = loc_[COL_IDX];
			memory_->rinfo.rloc[idx_][ROW_IDX] = loc_[ROW_IDX];
		}
	}

	int main() {
		size_t target = 0;
		// TODO: replace -1 with poision pill
		while (target != -1) {
			target = robotPosQueue.get();
			go(target, target);
		}
		removeMe();
		return 0;
	}
};


#endif //AMAZOOM_ROBOT_H
