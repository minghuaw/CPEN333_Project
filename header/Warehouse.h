//
// Warehouse.h
//

#ifndef AMAZOOM_WAREHOUSE_H
#define AMAZOOM_WAREHOUSE_H

#include "Amazoom.h"
#include "Robot.h"
#include "InventoryDatabase.h"
#include "ManagerUI.h"
#include "Truck.h"

#include <cpen333/process/semaphore.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cpen333/thread/semaphore.h>
#include <iostream>
#include <cpen333\process\subprocess.h>
#include <cpen333/util.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <string>
#include <random>
#include <ctime>

/**
* Reads layoutinfo from a filename and populates the layoutinfo
* Create the Layout object that contains the cell information
* @param filename file to load layout from
* @param linfo layout info to populate
*/
void loadLayoutInfo(const std::string& filename, LayoutInfo& linfo) {
	// initialize number of rows and columns
	linfo.rows = 0;
	linfo.cols = 0;

	std::ifstream fin(filename);
	std::string line;

	// read layout file
	if (fin.is_open()) {
		int row = 0;  // zeroeth row
		while (std::getline(fin, line)) {
			int cols = line.length();
			if (cols > 0) {
				// longest row defines columns
				if (cols > linfo.cols) {
					linfo.cols = cols;
				}
				for (size_t col = 0; col<cols; ++col) {
					linfo.layout[col][row] = line[col];
				}
				++row;
			}
		}
		linfo.rows = row;
		fin.close();
	}
}

class Warehouse{
private:
	// shared information layout info and robot info, shared among LayoutGUI and Warehouse processes
    cpen333::process::shared_object<SharedData> memory_;    
    cpen333::process::mutex ly_mutex_;
	int magicKey;

	// bays
    cpen333::process::semaphore loadingBay;
    cpen333::process::semaphore unloadingBay;

	// special locations in layout
	int loading[2];		// loading bay location
	int unloading[2];   // loading bay location
	int home[2];		// robot home location

	// trucks
	std::vector<DeliveryTruck*> dTrucks; //delivery trucks
	std::vector<RestockTruck*> rTrucks; //restock trucks

	// inventory database
    InventoryDatabase& inventory;
	cpen333::process::mutex db_mutex;

	// robots
	std::vector<Robot*> robots;

	//TruckQueue truckQueue;
	int orderCounter = 1;		// keep track of manager order

	// store a map of items and its weight
	std::map<std::string, double> itemName2weight;			// map item name to weight

public:
	// Queues
	OrderQueue robotOrderQueue;
	OrderQueue truckOrderQueue;
	ItemQueue loadingQueue;
	ItemQueue unloadingQueue;

	/**
	*  Warehouse constructor
	*  start the Warehouse system
	* initialize all inter-thread communication queues
    * Initialize magicKey to LAYOUT_MAGIC_KEY
    */
    Warehouse(InventoryDatabase& inventory):memory_(WAREHOUSE_MEMORY_NAME),ly_mutex_(WAREHOUSE_MEMORY_MUTEX_NAME),\
		magicKey(LAYOUT_MAGIC_KEY), db_mutex(DB_MUTEX_NAME),inventory(inventory),\
		loadingBay(LOADING_BAY_NAME, LOADING_BAY_SEM_RESOURCE),\
		unloadingBay(UNLOADING_BAY_NAME, LOADING_BAY_SEM_RESOURCE) {
		{
			std::unique_lock<decltype(ly_mutex_)> lock(ly_mutex_);
			// load layout, init robots
			initSpecialLocations();
			init_robots();
		}
	}

	/**
	* initialize the special locations, such as loading bay, unloading bay and home in the shared memeory
	* @param linfo  layout info in shared memory
	*/
	void initSpecialLocations() {
		LayoutInfo& linfo = memory_->linfo;
		loading[COL_IDX] = -1;
		loading[ROW_IDX] = -1;
		unloading[COL_IDX] = -1;
		unloading[ROW_IDX] = -1;
		home[COL_IDX] = -1;
		home[ROW_IDX] = -1;

		// initialize loading bay location
		for (int r = 0; r < linfo.rows && loading[0] < 0; ++r) {
			for (int c = 0; c < linfo.cols; ++c) {
				char ch = linfo.layout[c][r];
				if (ch == LOADINGBAY_CHAR) {
					loading[0] = c;
					loading[1] = r;
					break;
				}
			}
		}

		// initialize unloading bay location
		for (int r = 0; r < linfo.rows && unloading[0] < 0; ++r) {
			for (int c = 0; c < linfo.cols; ++c) {
				char ch = linfo.layout[c][r];
				if (ch == UNLOADINGBAY_CHAR) {
					unloading[0] = c;
					unloading[1] = r;
					break;
				}
			}
		}

		// initialize robot home location
		for (int r = 0; r < linfo.rows && home[0] < 0; ++r) {
			for (int c = 0; c < linfo.cols; ++c) {
				char ch = linfo.layout[c][r];
				if (ch == HOME_CHAR) {
					home[0] = c;
					home[1] = r;
					break;
				}
			}
		}
	}

	/**
	* init all robots starting location at home position
	* @param rinfo robot info in shared memory
	*/
	void init_robots() {
		RobotInfo& rinfo = memory_->rinfo;
		rinfo.nrobots = 0;
		for (size_t i = 0; i<MAX_ROBOTS; ++i) {
			rinfo.rloc[i][COL_IDX] = home[COL_IDX];
			rinfo.rloc[i][ROW_IDX] = home[ROW_IDX];
		}
	}

	/**
	* spawn a new robot and move to another thread
	* @param quantity	number of robots to be spawned
	* @return true if robot spawned, false if there are already MAX_ROBOT robots running
	*/
	bool spawn_robot(int quantity) {
		for (int i = 0; i < quantity; i++) {
			if (robots.size() < MAX_ROBOTS) {
				Robot* r = new Robot(inventory, robotOrderQueue);
				robots.push_back(r);
				r->start();
			}
			else {
				return false;
			}
		}
		return true;
	}

	/**
	* Remove robots
	*/
	bool remove_robot(int quantity) {
		for (int i = 0; i < quantity; i++) {
			if (robots.size() != 0) {
				robotOrderQueue.addOrder(Order(POISION_ID));
				robots.pop_back();
			}
			else {
				return false;
			}
		}
		return true;
	}
	/**
	* spawn a new delivery truck and move to another thread
	* @param quantity	number of delivery trucks to be spawned
	* @return true if robot spawned, false if there are already MAX_TRUCKS robots running
	*/
	bool spawn_dtruck(int quantity) {
		for (int i = 0; i < quantity; i++) {
			if (dTrucks.size() < MAX_TRUCKS) {
				DeliveryTruck* d = new DeliveryTruck(loadingQueue);
				dTrucks.push_back(d);
				d->start();
			}
			else {
				return false;
			}
		}
		return true;
	}

	/**
	* spawn a new restock truck and move to another thread
	* @param quantity	number of delivery trucks to be spawned
	*/
	bool spawn_rtruck(int quantity) {
		for (int i = 0; i < quantity; i++) {
			if (rTrucks.size() < MAX_TRUCKS) {
				RestockTruck* r = new RestockTruck(truckOrderQueue, unloadingQueue);
				rTrucks.push_back(r);
				r->start();
			}
			else {
				return false;
			}
		}
		return true;
	}

    /**
     * start remote server in a separate console/process
     */
    void startRemoteServer(){}

	/**
	* close warehouse
	*/
	void close() {
		{	// close all other processes
			std::unique_lock<decltype(ly_mutex_)> lock(ly_mutex_);
			memory_->quit = true;
		}
		// join all robot threads
		for (auto& robot: robots) {
			robot->join();
		}
		// join all truck threads
		for (auto& d : dTrucks) {
			d->join();
		}
		for (auto& r : rTrucks) {
			r->join();
		}
	}

	/**
	* return the coordinate and the corresponding quantity of item by finding item name. Executed after a quote is received.
	* @param itemName	name of item to be found
	* @param quantity	number of items needed
	* @return a vector of coordinate and quantity pairs
	*/
	std::vector<std::pair<Coordinate, int>> findLocation(std::pair<std::string, int> itemQuan) {}

	/**
	* confirm a restocking quote and convert it to an order (toOrder()), the central computer will then
	* place the order in the truckOrderQueue, and put a void order in robotOrderQueue
	* so that robots know to go to trucks
	* @param quote		restocking quote to be confirmed by warehouse
	* @param outorder	output order
	* @return always return true
	*/
	bool confirmRestockingQuote(Quote quote, Order& outorder) {}

	/**
	* confirm if a client quote can be processed after checking stock, if confirmed, convert quote to an order
	* and call "placeOrderInQueue" to put the order in the robotOrderQueue. This is being called from the socket
	* switch case after receiving a quote from the RemoteWebServer
	* @param quote		client quote to be confirmed by warehouse
	* @param outorder	output order
	* @return true if a restocking quote can be confirmed, false otherwise
	*/
	static bool confirmClientQuote(Quote& quote, InventoryDatabase& inventory_, int& orderCounter, Order& outorder) {
		std::string itemName;
		int num;
		bool success = true;
		std::string orderID;

		for (auto& pair : quote.quote_) {
			itemName = pair.first;
			num = pair.second;

			// find item
			int quantity = inventory_.findItemQuantity(itemName);
			if (quantity == ITEM_NOT_FOUND) {
				success &= false;
			}
			else {
				if (quantity >= num) {
					success &= true;
				}
				else {
					success &= false;
				}
			}
		}

		if (success) {
			orderID = "C0" + std::to_string(orderCounter);
			Order order = Warehouse::toOrder(std::ref(orderID), std::ref(quote),
				std::ref(inventory_), OrderType::CLIENT);
			outorder = order;
		}
		
		return success;
	}

	/**
	* place an order in the robotOrderQueue or truckOrderQueue depend on the order type
	* @param order		order to be placed
	*/
	void placeOrderInQueue(Order order) {}

	/**
	* find the weight of an order
	* @param order		order to be checked
	* @return weight of order
	*/
	double findOrderWeight(Order& order) {}

	/**
	* find the weight of an order
	* @param order		order to be checked
	* @return weight of order
	*/
	OrderStatus findOrderStatus(Order& order) {}

	/**
	* remove an order only if the order is RECEIVED
	* @param orderID	ID of an order
	* @return true if order is removed, false otherwise
	*/
	bool removeOrder(Order& order) {}

	/**
	* show manager UI, loop until user quits
	*/
	void showManagerUI() {
		// keep reading commands until the user quits
		char cmd = 0;
		while (cmd != MANAGER_QUIT) {
			print_mainmenu();

			// get menu entry
			std::cin >> cmd;
			// gobble newline
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			switch (cmd) {
			case ADD_ROBOT:
			{
				int quantity = 0;
				std::cout << "Enter number of robots to be added" << std::endl;
				std::cin >> quantity;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (!spawn_robot(quantity)) {
					std::cout << "error" << std::endl;
				}
			}
				break;
			case REMOVE_ROBOT:
				// remove robot
				if (!remove_robot(1)) {
					std::cout << "error" << std::endl;
				}
				break;
			case ADD_DELIVERY_TRUCK:
			{
				int quantity = 0;
				// add delivery truck
				std::cout << "Enter number of delivery trucks to be added" << std::endl;
				std::cin >> quantity;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (!spawn_dtruck(quantity)) {
					std::cout << "error" << std::endl;
				}
			}
				break;
			case ADD_RESTOCK_TRUCK:
			{
				int quantity = 0;
				// add restock truck
				std::cout << "Enter number of restock trucks to be added" << std::endl;
				std::cin >> quantity;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (!spawn_rtruck(quantity)) {
					std::cout << "error" << std::endl;
				}
			}
				break;
			case ADD_ORDER:
			{
				// create a new quote
				Quote quote;
				// order menu starts
				char order_cmd = 0;		
				while (order_cmd != MANAGER_QUIT && order_cmd != CONFIRM_ORDER) {
					int quantity = 0;
					std::string s = "";

					print_ordermenu();
					// get menu entry
					std::cin >> order_cmd;
					// gobble newline
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

					switch (order_cmd) {
					case ADD_ITEM:
						// add iteminfo to order
						std::cout << "Enter ID of item to be added" << std::endl;
						std::cin >> s;
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						if (inventory.findItemWeight(s) == ITEM_NOT_FOUND) {		// check if there is weight associated with item
							std::cout << "Item does not exist" << std::endl;
						}
						else {
							std::cout << "Enter number of items to be added" << std::endl;
							std::cin >> quantity;
							std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
							quote.addItemQuote(s, quantity);
						}
						break;
					case CONFIRM_ORDER:
						{
							std::string orderID = "M0" + std::to_string(orderCounter);
							Order order = Warehouse::toOrder(std::ref(orderID), std::ref(quote),
								std::ref(inventory), OrderType::MANAGER);
							orderCounter++;
							// push manager order to order queue
							robotOrderQueue.addOrder(order);
							std::cout << "order added to queue" << std::endl;
						}
						break;
					case MANAGER_QUIT:
						break;
					default:
						std::cout << "Invalid command number " << cmd << std::endl << std::endl;
					}
				}

			}
				break;
			case MANAGER_QUIT:
				std::cout << "Closing warehouse" << std::endl;
				break;
			default:
				std::cout << "Invalid command number " << cmd << std::endl << std::endl;
			}

			cpen333::pause();
		}
	}

	/**
	* convert a quote to order
	* @param id			order ID
	* @param quote		quote to convert
	* @param inventory	inventory database
	* @param type		type of order
	* @return order object
	*/
	static Order toOrder(std::string & id, Quote & quote, InventoryDatabase & inventory, OrderType type) {
		std::string name;
		int num;
		double weight;
		Coordinate loc;
		ItemInfo info;
		std::vector<std::pair<ItemInfo, int>> outorder;

		for (std::pair<std::string, int>& pair : quote.quote_) {
			name = pair.first;
			num = pair.second;
			weight = inventory.findItemWeight(std::ref(name)); // get item weight
			loc = inventory.findItemLocation(std::ref(name)); // get item location
			info = ItemInfo(name, weight, loc); // create item info object
			outorder.push_back(std::pair<ItemInfo, int>(info, num)); // push to vector
		}

		return Order(id, outorder, type);
	}
};

#endif //AMAZOOM_WAREHOUSE_H
