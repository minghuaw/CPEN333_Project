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

class Robot {
private:
    cpen333::process::shared_object<SharedData> memory_;    // warehouse info
    cpen333::process::mutex mutex_;

    std::string ID;
    char col;
    int row;
    Order& order;                       // TODO: maybe wrong? should it be order?
    double capacity;
    std::vector<std::pair<Item,int>> items;
    double currWeight;
    ItemQueue unloadingQueue;
    OrderQueue robotOrderQueue;
    std::mutex db_mutex;                // for database protection
    InventoryDatabase& database;        // shared database
    Layout warehouseLayout;             // local copy of layout


public:
    Robot(InventoryDatabase& database, std::string ID): database(database),ID(ID) {}

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
    bool go(char col,int row){}

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
};


#endif //AMAZOOM_ROBOT_H

//=================================================
//=================================================
/**
 *@file Order.h
 *
 * This file contains the definition of an order in the database
 *
 */

#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <iostream>
#include <vector>
#include "Item.h"

enum OrderType {
  CLIENT,
  MANAGER,
  UNKNOWN
};

enum OrderStatus {
  RECEIVED,		// when computer creates the order
  PROCESSING,	// robot starts to pick it up and put on truck
  SHIPPED		// robot finishes the order
};

class Order {
 private:
  std::string ID;								// order ID
  OrderStatus status;							// status of order
  double weight;								// total weight of the order
  OrderType type;								// type of order, either from client or manager
  std::vector<std::pair<ItemInfo,int>> items;	// item info and quantity
  std::string truckID;

public:
    /**
     * default constructor
     */
    Order(){}

    /**
     *
     * @param ID order id in string format
     */
    Order(std::string ID) : ID(ID) {}

    /**
    * return the status of an order
    * @return 	status of order
    */
    OrderStatus returnOrderStatus() {
    }
   
    /**
    * change the status of an order
    * @param newStatus	status of an order to be changed
    */
    void changeOrderStatus(OrderStatus newStatus) {
    }
   
    /**
    * return the ID of an order
    * @return 	ID of order in string format
    */
    std::string returnOrderID() {
    }
   
    /**
    * assign truck ID to order
    * @return 	ID of order in string format
    */
    void assignTruckID(std::string truckID) {
    }

    /**
    * Convert an order to string format
    * @return 	order in string format
    */
    std::string toString() const {
    }

    /**
     * get an item info from the order and remove pair from the vector from the back
     * @param outItemInfo  output with item info and quantity
     * @return true if there are still pairs in the vector
     */
    bool getItemInfo(std::pair<ItemInfo,int>& outItemInfo) {}
   

    /**
    * less-than operator for comparisons, sort by order ID
    * @param a	order to be compared
    * @param b	order to be compared
    * @return 	true if order a smaller than order b, false otherwise
    */
    friend bool operator<(const Order& a, const Order& b) {
    if (a.ID < b.ID) {
        return true;
    }
    else {
        return false;
    }
    }

   /**
   * equal-to operator for comparisons, ID must match 
   * @return 	true if order IDs are the same, false otherwise
   */
  friend bool operator==(const Order& a, const Order& b) {
    return (a.ID == b.ID);
  }

  /**
   * not-equal-to operator for comparisons
   * @return 	true if order IDs are not the same, false otherwise
   */
  friend bool operator!=(const Order& a, const Order& b) {
    return !(a == b);
  }

  /**
   * overloaded stream operator for printing, std::cout << Order
   * @return 	order in string format
   */
  friend std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.toString();
    return os;
  }

};

#endif //ORDER_H

//=================================================
//=================================================
//
// Layout.h
// display warehouse, separate process and shared memory
//

#ifndef AMAZOOM_LAYOUT_H
#define AMAZOOM_LAYOUT_H

#define MAZE_MEMORY_NAME "shared_memory"
#define MAZE_MUTEX_NAME "shared_memory_mutex"

#define WALL_CHAR 'X'
#define EMPTY_CHAR ' '
#define EXIT_CHAR 'E'

#define COL_IDX 0
#define ROW_IDX 1

#define ROBOT_HOME_COL = A
#define ROBOT_HOME_ROW = 1

#define MAX_LAYOUT_COL 10
#define MAX_LAYOUT_ROW 10
#define MAX_ROBOTS   50

#define MAGIC_NUMBER 1373858591

#include "Cell.h"
#include "Coordinate.h"
#include <map>

struct LayoutInfo {
    int rows;           // rows in warehouse
    int cols;           // columns in warehouse
    char layout[MAX_LAYOUT_COL][MAX_LAYOUT_ROW];  // warehouse storage
};

struct RobotInfo {
    int nrobots;      // number robots
    int rloc[MAX_ROBOTS][2];   // robot locations [col][row]
};

struct SharedData {
    LayoutInfo linfo;    // layout info
    RobotInfo rinfo;  // robot info
    bool quit;         // tell everyone to quit
    int  magic;        // magic number for detecting initialization
};

class Layout{
    protected:
        std::map<std::string, Cell> coordinate2cell;        // map coordinate to storage cells
};

#endif //AMAZOOM_LAYOUT_H

//=================================================
//=================================================
//
//  DynamicQueue.h
//

#ifndef DYNAMICQUEUE_H
#define DYNAMICQUEUE_H

#include "Order.h"
#include "Item.h"
#include "Quote.h"
#include "TruckInfo.h"
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>

template <class T>

class DynamicQueue{
	private:
		std::deque<T> buff_;
		std::mutex mutex_;
		std::condition_variable cv_;
		
	public:
		/**
		* Creates the dynamic queue
		*/
		DynamicQueue() :
			buff_(), mutex_(), cv_(){}
			
		/**
		* Adds an template obj to the queue
		* @param template obj to add
		*/
		void add(T& obj) {
			{
				std::lock_guard<std::mutex> lock(mutex_);
				buff_.push_back(obj);
			}
			cv_.notify_all();
		}

		/**
		* Retrieve the next order template obj in the queue
		* @return next available template obj
		*/
		T get(){
			T out;
			{
			std::unique_lock<std::mutex> lock(mutex_);
				while (buff_.size()==0) {
					cv_.wait(lock);
				}
				// get first item in queue
				out = buff_.front();
				buff_.pop_front();
			}

			return out;
		}

};

class ItemQueue: public DynamicQueue{
	public:
		/**
		* Creates the dynamic item queue
		*/
		ItemQueue(): DynamicQueue() {}
};

class QuoteQueue: public DynamicQueue{
	public:
		/**
		* Creates the dynamic quote queue
		*/
		QuoteQueue(): DynamicQueue() {}
};

class TruckQueue: public DynamicQueue{
	public:
		/**
		* Creates the dynamic truck queue
		*/
		TruckQueue(): DynamicQueue() {}
};

class MessageQueue: public DynamicQueue{
	public:
		/**
		* Creates the dynamic message queue
		*/
		MessageQueue(): DynamicQueue() {}
};

class OrderQueue: public DynamicQueue{
	private:
		int processIndex;
	public:
		/**
		* Creates the dynamic order queue
		*/
		OrderQueue(): DynamicQueue() {}
		
		/**
		* Try to get an order that weighs less than or equal to the specified weight
		* @param weight		maximum weight of order
		* @param outorder	the order found that meets the requirement
		* @return			true if an order less than or equal to specified weight can be found, false otherwise
		*/
		bool tryGet(double weight, Order& outorder){
		}
		
		/**
		* Swap two orders in queue at specified indexes
		* @param ind1		index of first order
		* @param ind2		index of second order
		*/
		bool swapOrder(int ind1, int ind2){
		}
		
		/**
		* search order with ID
		* @param orderID	ID of order
		* @param outorder	the ordre found to match the ID
		* @return True if an order if found, false otherwise
		*/
		bool searchOrderID(std::string orderID, Order& outorder){
		}
};

#endif // DYNAMICQUEUE_H

//=================================================
//=================================================
//
// UserInterface.h
//

#ifndef AMAZOOM_USERINTERFACE_H
#define AMAZOOM_USERINTERFACE_H

#include "Order.h"
#include <vector>
#include <string>
#include "Quote.h"

class UserInterface{
private:
    Quote selectedItems;
public:
    /**
     * default constructor
     */
    UserInterface(){}

    /**
     * query the status of an order
     * for manager, check shared object directly, for client, send to server
     * @param orderID   the ID of order
     * @return order status
     */
    virtual OrderStatus queryOrder(std::string orderID){}

    /**
     * add a quote of selected items, clear selected items after sending
     * for manager, push to queue, for client, send to server
     */
    virtual addQuote(){}

    /**
     * select an item to add to selected items
     * @param itemName  the name of item
     * @param quantity  the quantity of item
     */
    void selectItem(std::string itemName, int quantity){}

    /**
     * display options, constantly called within a while loop
     */
    void displayUI() {}
};

class ManagerUI: public UserInterface {
private:
    cpen333::process::shared_object<SharedData> memory_;

public:
    /**
     * default constructor, calls default constructor of superclass
     */
    ManagerUI():UserInterface(){}

    /**
    * cancel an order, if not processing or shipped
    * @param orderID ID of order
    * @return true if order is cancelled
    */
    bool cancelOrder(std::string orderID) {}

    /**
     * check stock of items in warehouse
     * @param itemName
     * @return quantity of itemName, return NAN if item not existed
     */
    int checkStock(std::string itemName) {}

    /**
     * print the name of low stock item on screen
     * @param itemName  name of item low in stock
     * @param quantity quantity of item
     */
    void alertLowStock(std::string itemName, int quantity) {}

    /**
     * create another thread for delivery truck
     */
    void addDeliveryTruck() {}

    /**
     * create another thread for restock truck
     */
    void addRestockTruck() {}

    /**
     * create another thread for robot, modify shared variable
     */
    void addRobot() {}

    /**
     * remove a robot, join thread and modify shared variable
     * @param robotID id of robot to be removed
     */
    void removeRobot(std::string robotID) {}

    /**
     * push quote in quoteQueue
     */
    void sendQuote() {}
};

class ClientUI: UserInterface {
private:
    pen333::process::socket socket;
public:
    /**
     * default constructor
     * calls default constructor of superclass
     */
    ClientUI():UserInterface(){}
    /**
    * cancel an order, send to server
    * @param orderID ID of order
    */
    void do_cancel(std::string orderID) {}

    /**
     * connect to server at port
     * @param port port number of server
     * @return true if connected
     */
    bool connectServer(int port) {}

    /**
     * send quote to server
     */
    void sendQuote() {}
};

#endif //AMAZOOM_USERINTERFACE_H

//=================================================
//=================================================
//
// DeliveryTruck.h
//

#ifndef AMAZOOM_DELIVERYTRUCK_H
#define AMAZOOM_DELIVERYTRUCK_H

#include <string>
#include <vector>
#include "Amazoom.h"
#include "Truck.h" 
#include "DynamicQueue.h"
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
    DeliveryTruck(std::string truckID, double cap, std::vector<Item> items, TruckQueue& truckQ, ItemQueue& loadQ) :Truck(truckID, cap, items, truckQ){}

    /**
     * @brief wait at the loading bay until both conditions are true
     * 1. truck is full enough
     * 2. all orders are completely loaded (a poison pill will be given by
     * robot to indicate the end of an order)
     */
    void waitTillFull(){}

    /**
     * @brief taking items from loadingQueue
     *  
     */
    void fill(){}
};
#endif //AMAZOOM_DELIVERYTRUCK_H

//=================================================
//=================================================
//
//  Amazoom.h
//  common definitions
//

#ifndef AMAZOOM_H
#define AMAZOOM_H

#define DB_MUTEX_ID "DB_MUTEX"
#define SHARED_LAYOUT_ID "LAYOUT"
#define LAYOUT_MAGIC_KEY "99"
#define LOADING_BAY_SEM_RESOURCE 1 // only one loading dock is available
#define LOADING_BAY_NAME "loading"
#define UNLOADING_BAY_NAME "unloading"
#define LAYOUT_0 "../data/layout0.txt"
#define WAREHOUSE_NUM 2
#define MAX_ROBOT 50

#endif // AMAZOOM_H
//=================================================
//=================================================
//
// RemoteWebServer.h
//

#ifndef AMAZOOM_REMOTEWEBSERVER_H
#define AMAZOOM_REMOTEWEBSERVER_H

#include <cpen333/process/mutex.h>
#include <cpen333/process/socket.h>
#include <mutex>
#include <string>


class RemoteWebServer{
private:
    std::mutex mutex_;              // mutex for multi client connection
    std::deque<std::string> msg2warehouseQueue;
    std::deque<std::string> msg2clientQueue;
    cpen333::process::socket_server server;
    cpen333::process::socket client;
    cpen333::process::socket whComputer[WAREHOUSE_NUM];
    WarehouseComputerAPI api;
public:
    /**
     * Constructor, initializes socket server and warehouseComputer API
     */
    RemoteWebServer(){}

    /**
     * Main thread function for handling communication with a single remote
     * client. Message received from client socket is appended with client id
     * and  pushed onto msg2warehouseQueue.
     * Reads msg from msg2clientQueue, if the msg match the client ID,
     * pass the msg to client over client socket
     * @param api WarehouseComputerAPI for sending and recving messages from warehouseComputer
     * @param id client id
     */
    void serviceClient(int id){}

    /**
     * Main thread function for handling communication with a single remote warehouse computer
     * Message on the msgQueue is popped and sent to warehouseComputer on whComputer sockets
     * @param api warehouseComputreAPI
     * @param id warehouse ID
     */
    void serviceWarehouse(WarehouseComputerAPI& api, int id){}
};

class WarehouseComputerAPI{
private:
    cpen333::process::socket client;
public:
    /**
     * Constructor for WarehouseComputerAPI
     * @param client
     */
    WarehouseComputerAPI(cpen333::process::socket&& client){}

    /**
     * send JSON string message to warehouseComputer
     */
    void sendMessage(){}

    /**
     * recv JSON string message from warehouseComputer
     */
    void recvMessage(){}

};

#endif //AMAZOOM_REMOTEWEBSERVER_H

//=================================================
//=================================================
//
// TruckInfo.h
//

#ifndef AMAZOOM_TRUCKINFO_H
#define AMAZOOM_TRUCKINFO_H

#include <string>
class TruckInfo{
private:
    std::string ID;
    double capacity;
    std::string action;
public:

};
#endif //AMAZOOM_TRUCKINFO_H

//=================================================
//=================================================
/**
 * @file Message.h
 *
 * This file contains all message-related objects, independent of the specific API
 *
 * This middle layer allows us to abstract away many of the communication details,
 * allowing us to focus on the core functional implementation.
 *
 */
#ifndef PROJECT_MESSAGES_H
#define PROJECT_MESSAGES_H

#include "Order.h"
#include "Quote.h"
#include <string>
#include <vector>

/**
 * Types of messages that can be sent between client/server
 */
enum MessageType {
    ADD,
    ADD_RESPONSE,
    REMOVE,
    REMOVE_RESPONSE,
    REMOVE_ITEM,
    REMOVE_ITEM_RESPONSE,
    SEARCH,
    SEARCH_RESPONSE,
    SEARCH_ITEM,
    SEARCH_ITEM_RESPONSE,
    GOODBYE,
    UNKNOWN
};

// status messages for response objects
#define MESSAGE_STATUS_OK "OK"
#define MESSAGE_STATUS_ERROR "ERROR"

/**
 * Base class for messages
 */
class Message {
public:
    virtual MessageType type() const = 0;
};

/**
 * Base class for response messages
 */
class ResponseMessage : public Message {
public:
    const std::string status;
    const std::string info;

    explicit ResponseMessage(const std::string &status,
                    const std::string &info = "") :
            status(status), info(info) {}

};

/**
 * Add a quote to central computer
 */
class AddMessage : public Message {
public:
    const Quote quote;

    explicit AddMessage(const Quote &quote) : quote(quote) {}

    MessageType type() const override {
        return MessageType::ADD;
    }
};

/**
 * Response to adding a quote to central computer
 */
class AddResponseMessage : public ResponseMessage {
public:
    const AddMessage add;

    AddResponseMessage(const AddMessage &add, std::string status, std::string info = "") :
            ResponseMessage(status, info), add(add) {}

    MessageType type() const override {
        return MessageType::ADD_RESPONSE;
    }
};

/**
 * Remove an order from the central computer
 */
class RemoveMessage : public Message {
public:
    const std::string orderID;

    RemoveMessage(const std::string &orderID) : orderID(orderID) {}

    MessageType type() const override {
        return MessageType::REMOVE;
    }
};

/**
 * Response to removing an order from the central computer
 */
class RemoveResponseMessage : public ResponseMessage {
public:
    const RemoveMessage remove;

    RemoveResponseMessage(const RemoveMessage &remove, std::string status, std::string info = "") :
            ResponseMessage(status, info), remove(remove) {}

    MessageType type() const override {
        return MessageType::REMOVE_RESPONSE;
    }
};

/**
 * Remove an order from the central computer
 */
class RemoveItemMessage : public Message {
public:
    const std::string orderID;
    const std::string itemName;

    RemoveItemMessage(const std::string &orderID, const std::string &itemName)
                        : orderID(orderID), itemName(itemName){}

    MessageType type() const override {
        return MessageType::REMOVE_ITEM;
    }
};

/**
 * Response to removing an order from the central computer
 */
class RemoveItemResponseMessage : public ResponseMessage {
public:
    const RemoveItemMessage removeItem;

    RemoveItemResponseMessage(const RemoveItemMessage &removeItem, std::string status, std::string info = "") :
            ResponseMessage(status, info), removeItem(removeItem) {}

    MessageType type() const override {
        return MessageType::REMOVE_ITEM_RESPONSE;
    }
};

/**
 * Search the order in the central computer using regular expressions
 */
class SearchMessage : public Message {
public:
    const std::string orderID;

    SearchMessage(const std::string &orderID) :
            orderID(orderID) {}

    MessageType type() const override {
        return MessageType::SEARCH;
    }
};

/**
 * Response to a central computer order search
 */
class SearchResponseMessage : public ResponseMessage {
public:
    const SearchMessage search;
    const std::string results;

    SearchResponseMessage(const SearchMessage &search, const std::string &results,
                          const std::string &status, const std::string &info = "") :
            ResponseMessage(status, info), search(search), results(results) {}

    MessageType type() const override {
        return MessageType::SEARCH_RESPONSE;
    }
};

/**
 * Search the central computer using regular expressions
 */
class SearchItemMessage : public Message {
public:
    const std::string itemName;

    SearchItemMessage(const std::string &itemName) :
            itemName(itemName) {}

    MessageType type() const override {
        return MessageType::SEARCH_ITEM;
    }
};

/**
 * Response to a central computer order search
 */
class SearchItemResponseMessage : public ResponseMessage {
public:
    const SearchItemMessage searchItem;
    const std::string results;

    SearchItemResponseMessage(const SearchItemMessage &searchItem, const std::string &results,
                          const std::string &status, const std::string &info = "") :
            ResponseMessage(status, info), searchItem(searchItem), results(results) {}

    MessageType type() const override {
        return MessageType::SEARCH_ITEM_RESPONSE;
    }
};

/**
 * Goodbye message
 */
class GoodbyeMessage : public Message {
public:
    MessageType type() const override {
        return MessageType::GOODBYE;
    }
};

#endif //PROJECT_MESSAGES_H

//=================================================
//=================================================
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

//=================================================
//=================================================
/**
 * @file WarehouseComputer.h
 * This document defines the attribute and public operation of WarehouseComputer class
 * 
 */


#ifndef WHAREHOUSE_COMPUTER_H
#define WHAREHOUSE_COMPUTER_H

#include <string>
#include <iostream>
#include <vector>

#include "InventoryDatabase.h"
#include "DynamicQueue.h"
#include "Layout.h"
#include "Coordinate.h"

class WarehouseComputer{
    private:
		InventoryDatabase database;
		OrderQueue robotOrderQueue;
		OrderQueue truckOrderQueue;
		ItemQueue loadingQueue;
		ItemQueue unloadingQueue;
		TruckQueue truckQueue;
		Layout warehouseLayout;
		std::mutex db_mutex_;			// synchronization of database
	
    public:
		/**
		 * Constructor for Coordinate class
		 */
		WarehouseComputer(){}
		
		/**
		 * return the coordinate and the corresponding quantity of item by finding item name. Executed after a quote is received.
		 * @param itemName	name of item to be found
		 * @param quantity	number of items needed
		 * @return a vector of coordinate and quantity pairs
		 */
		std::vector<std::pair<Coordinate, int>> findLocation(std::pair<std::string, int> itemQuan) {}

		/**
		 * confirm a restocking quote and convert it to an order, the central computer will then
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
		bool confirmClientQuote(Quote quote, Order& outorder) {}

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
		
};

#endif // WAREHOUSE_COMPUTER
//=================================================
//=================================================
//
// Quote.h
//

#ifndef AMAZOOM_QUOTE_H
#define AMAZOOM_QUOTE_H

#include <string>

class Quote {
private:
    std::vector<std::pair<std::string,int>> quotes;
public:
    /**
     * default constructor
     */
    Quote(){}

    /**
     * constructor
     * @param itemName single item name
     * @param quantity item wanted wanted
     */
    Quote(std::string itemName, int quantity){}

    /**
     * constructor
     * @param itemVector vector of std::pair<std::string itemName, int quantity>
     */
    Quote(std::vector<std::pair<std::string, int >> &itemVector){}

    /**
     * add a quote of item to the quote
     * @param itemName name of item
     * @param quantity quantity of item
     */
    void addItemQuote(std::string itemName, int quantity){}

    /**
     * get an item quote from the quotes and remove pair from the vector from the back
     * @param outItemQuote  single item quote output, with item name and quantity
     * @return true if there are pairs in the vector
     */
    bool getItemQuote(std::pair<std::string,int>& outItemQuote) {}
};

#endif //AMAZOOM_QUOTE_H

//=================================================
//=================================================
//
//  Cell.h
//

#ifndef CELL_H
#define CELL_H

#include "Item.h"

class Cell {
private:
    double capacity;            //capacity of the cell
    double currWeight;          // current weight of all items in the cell
};

#endif // CELL_H
//=================================================
//=================================================
//
//  Coordinate.h
//

#ifndef COORDINATE_H
#define COORDINATE_H

#include <string>

class Coordinate {
private:
    char col;               // column number "A, B, C ..."
    int row;                // row number "1, 2, 3 ....."
    char side;              // side on the shelf "L, R"
    int shelf;              // shelf level "1, 2, 3 ...."
public:
    /**
     * default constructor
     */
    Coordinate(){}

    /**
     * Constructor for Coordinate class
     * @param col column number, captitalized char from 'A' to 'Z'
     * @param row row number, positive integer
     * @param side side, char of either 'L' or 'R'
     * @param shelf shelf level, positive integer
     */
    Coordinate(char col, int row, char side, int shelf) {}

    /**
     * return the col number
     * @return col number, char from 'A' to 'Z'
     */
    char getCol(){}

    /**
     * return row number
     * @return row number
     */
    int getRow(){}

    /**
     * return side
     * @return side, char 'L' or 'R'
     */
    char getSide(){}

    /**
     * return shelf level number
     * @return shelf level
     */
    int getShelf(){}

    /**
     * convert Coordinate to string
     * @return std::string in format "[col][row][side][shelf]"
     */
    std::string toString(){}

};

#endif
//=================================================
//=================================================
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
    void notifyLeaving(){}
};
#endif //CPEN333_PROJECT_TRUCK_H

//=================================================
//=================================================
//
//  Item.h
//
#ifndef PROJECT_ITEM_H
#define PROJECT_ITEM_H

#include <string>
#include <iostream>
#include "Coordinate.h"

class Item {
private:
    std::string ID;                                // item ID
    double weight;                                // total weight of the item

public:
    // TODO: how to construct the item????
    Item(const std::string ID, const double weight) :
            ID(ID), weight(weight) {}

    // TODO: what form?
    /**
     * Convert an item to string format
     * @return 	string "[ItemName][ItemWeight]"
     */
    std::string toString() const {
    }


    /**
    * less-than operator for comparisons, sort by item ID
    * @param a	item to be compared
    * @param b	item to be compared
    * @return 	true if item a smaller than item b, false otherwise
    */
    friend bool operator<(const Item &a, const Item &b) {
        return a.ID < b.ID;
    }

    /**
    * equal-to operator for comparisons, ID must match
    * @return 	true if item IDs are the same, false otherwise
    */
    friend bool operator==(const Item &a, const Item &b) {
        return (a.ID == b.ID);
    }

    /**
     * not-equal-to operator for comparisons
     * @return 	true if item IDs are not the same, false otherwise
     */
    friend bool operator!=(const Item &a, const Item &b) {
        return !(a == b);
    }

    /**
     * overloaded stream operator for printing, std::cout << Item
     * @return 	item in string format
     */
    friend std::ostream &operator<<(std::ostream &os, const Item &item) {
        os << item.toString();
        return os;
    }

};

class ItemInfo : public Item {
private:
    Coordinate location;
public:

    /**
     * Constructor for ItemInfo class, calls Item constructor
     * @param ID item ID, string
     * @param weight weight of the item, double
     * @param location location of the item, Coordinate
     */
    ItemInfo(std::string ID, const double weight, Coordinate location) : Item(ID, weight) {}

    /**
     * returns the location of the item
     * @return Coordinate
     */
    Coordinate getLocation() {}

    /**
     * set the location of the ItemInfo to loc
     * @param loc coordinate to set the ItemInfo location into
     */
    void setLocation(Coordinate loc){}
};


#endif //PROJECT_ITEM_H

//=================================================
//=================================================
//
//  InventoryDatabase.h
//

#ifndef AMAZOOM_INVENTORYDATABASE_H
#define AMAZOOM_INVENTORYDATABASE_H

#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include "Coordinate.h"
#include "Item.h"

class InventoryDatabase{
private:
    //TODO: supply our own comparator for ItemInfo's, as map internally is a binary tree
    std::map<ItemInfo, int> inventory;              // map of iteminfo to quantity
    std::multimap<std::string, Coordinate> itemLocation;    // multimap of itemLocations. One type of item may be store
                                                            // multiple locations
    std::mutex mutex_;              // internal mutex for thread safe operation
public:
    /**
     * default constructor, creates an empty inventory
     */
    InventoryDatabase(){}

    //TODO: Add another constructor

    /**
     * add additional entry with quantity if ItemInfo is not in inventory
     * add up the quantity if the ItemInfo is found in the inventory
     * @param itemInfo ItemInfo object of the target item,
     * @param quantity quantity of the itemiInfo to add into inventory
     * @return true if addition is successful/warehouse not full, false if warehouse full
     */
    bool addItemInfo(ItemInfo& itemInfo, int quantity){}

    /**
     * reduce quantity of an ItemInfo
     * @param itemInfo ItemInfo object of the target item,
     * @param quantity quantity of the itemiInfo to add into inventory
     * @return true if reduction is successful, false if reduction is unsuccessful
     */
    bool reduceItemInfoQuantity(ItemInfo& itemInfo, int quantity){}

    /**
     * Assign a new location to an ItemInfo
     * @param itemInfo itemInfo ItemInfo object of the target item
     * @param loc Coordinate of the new location
     */
    void assignItemLocation(ItemInfo& itemInfo, Coordinate loc){}

    /**
     * remove the current location of the ItemInfos
     * @param itemInfo itemInfo ItemInfo object of the target item
     * @param loc Coordinate of the new location
     */
    void remvItemLocation(ItemInfo& itemInfo, Coordinate loc){}

    /**
     * change the current location of the ItemInfos
     * @param itemInfo itemInfo ItemInfo object of the target item
     * @param loc Coordinate of the new location
     */
    void changeItemLocation(ItemInfo& itemInfo, Coordinate loc){}

    /**
     * try to find item by its name in the inventory
     * @param itemName item name, std::string
     * @return true if item is found, false if item is not found
     */
    bool findItem(std::string itemName){}

    /**
     * find the quantity of an item
     * @param itemName name of an item that is in the inventory
     * @return int quantity of the item
     */
    int returnItemQuantity(std::string itemName){}

    /**
     * find the weight of an item
     * @param itemName name of an item that is in the inventory
     * @return double weight of the item
     */
    double findItemWeight(std::string itemName){}

    /**
     * convert InventoryDatabase to a string
     * @return "InventoryDatabase"
     */
    std::string toString(){}

};
#endif //AMAZOOM_INVENTORYDATABASE_H

//=================================================
//=================================================
//
// Warehouse.h
//

#ifndef AMAZOOM_WAREHOUSE_H
#define AMAZOOM_WAREHOUSE_H

#include "Amazoom.h"
#include <cpen333/process/semaphore.h>
#include "Layout.h"

class Warehouse{
private:
    cpen333::process::shared_object<SharedData> memory_;    // warehouse info
    cpen333::process::mutex mutex_;

    cpen333::process::semaphore loadingBay;
    cpen333::process::semaphore unloadingBay;
    Layout warehouseLayout;
    int magicKey;

public:
    /**
     * Warehouse constructor
     * Initialize magicKey to LAYOUT_MAGIC_KEY
     */
    Warehouse(){}

    /**
     * load layout from text file
     */
    void loadLayout(const std::string& filename){}

    /**
     * display layout in a separate console/process
     */
    void showLayout(){}

    /**
     * start warehouse computer in a separate thread
     */
    void startWarehouseComputer(){}

    /**
     * start remote server in a separate console/process
     */
    void startRemoteServer(){}
};

#endif //AMAZOOM_WAREHOUSE_H

//=================================================
//=================================================
