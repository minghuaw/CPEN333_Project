//
// UserInterface.h
//

#ifndef AMAZOOM_USERINTERFACE_H
#define AMAZOOM_USERINTERFACE_H

#include <vector>
#include <string>
#include <cpen333\process\shared_memory.h>
#include <cpen333/console.h>
#include "Amazoom.h"
#include "Order.h"
#include "Layout.h"
#include "Quote.h"
#include "WarehouseComputerAPI.h"
#include "Message.h"
#include "jsonConverter.h"
#include "Quote.h"
#include <json.hpp>

#define ADD_END "N"
#define ADD_YES "Y"
#define ADD_N0 "N"
#define NOT_SELECTED "0"

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
    virtual OrderStatus queryOrder(std::string orderID){
		OrderStatus ret;
		return ret;
	}

    /**
     * add a quote of selected items, clear selected items after sending
     * for manager, push to queue, for client, send to server
     */
    virtual void addQuote(){}

    /**
     * select an item to add to selected items
     * @param itemName  the name of item
     * @param quantity  the quantity of item
     */
    void selectItem(std::string itemName, int quantity){}

    /**
     * display options, constantly called within a while loop
     */
    virtual void displayUI() {}
};

class ManagerUI: public UserInterface {
private:
    cpen333::process::shared_object<SharedData> memory_;

public:
    /**
     * default constructor, calls default constructor of superclass
     */
    ManagerUI():UserInterface(), memory_(WAREHOUSE_MEMORY_NAME) {}

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

class ClientUI: public UserInterface {
private:
    cpen333::process::socket socket_;
	WarehouseComputerAPI api_;
	std::string warehouse_id = NOT_SELECTED;

public:
    /**
     * default constructor
     * calls default constructor of superclass
     */
    ClientUI(int port = CLIENT_PORT):UserInterface(), socket_(LOCAL_HOST,port){
		std::cout << "Client UI and socket initialized" << std::endl;
		std::cout << "Client connecting..." << std::endl;
		std::cout.flush();
	}
	
	/**
	 * prompts user to add item to quote
	 */
	void do_add() {
		std::string itemName, numStr;
		int num;
		std::vector<std::pair<std::string, int>> items;

		std::string cmd;
		while (cmd != ADD_END) {
			int cnt = 1;
			std::cout << "Add item  " << cnt << std::endl;
			std::cout << "Item Name: ";
			std::getline(std::cin, itemName);
			std::cout << "Quantity (must be an integer): ";
			std::getline(std::cin, numStr);
			try {
				num = std::stoi(numStr);
			}
			catch (std::exception& e) {
				std::cerr << e.what();
			}
			
			items.push_back(std::pair<std::string, int>(itemName, num));

			std::cout << "Add another item? (Y/N): ";
			cnt++;
			//std::cin >> cmd;
			std::getline(std::cin, cmd);
		}

		std::cout << "Confirm items " << std::endl;
		std::cout << "Name | Quantity" << std::endl;
		int size = items.size();
		for (std::pair<std::string, int>& pair : items) {
			std::cout << pair.first << " | " << pair.second << std::endl;
		}		

		std::cout << "Quote content is correct (Y/N): ";
		std::getline(std::cin, cmd);
		//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (cmd == ADD_YES){
			// if user order is complete
			// create quote and send to server
			Quote quote(std::move(items));
			AddMessage add(quote, warehouse_id);
			api_.sendMessage(add);

			// wait for response
			std::unique_ptr<std::string> jsonStrPtr = api_.recvJSON();
			JSON jmsg = JSON::parse(*jsonStrPtr);
			
			//show response message
			std::cout << "Add order request response" << std::endl;
			if (jmsg[MESSAGE_STATUS] == MESSAGE_STATUS_OK) {
				std::cout << "Order is successfully placed!" << std::endl;
				std::cout << "Order number is: " << jmsg[MESSAGE_ORDER_ID] << std::endl;
			}
			else {
				std::cout << "Order placement failed" << std::endl;
			}

		}
		else {
			std::cout << "Cancelling quote \n quit to main menu" << std::endl;
		}
		cpen333::pause();
	}

	/**/
	void do_add(std::vector<std::string> strs) {}


	void do_remove() {
		std::string orderID;

		// user input order id
		std::cout << "Remove Order" << std::endl;
		std::cout << "Please enter order id: ";
		std::getline(std::cin, orderID);

		RemoveMessage remove(orderID, warehouse_id);
		api_.sendMessage(remove);

		// wait for response
		std::unique_ptr<std::string> jsonStrPtr = api_.recvJSON();
		JSON jmsg = JSON::parse(*jsonStrPtr);

		std::cout << "Remove order request response" << std::endl;
		if (jmsg[MESSAGE_STATUS] == MESSAGE_STATUS_OK) {
			std::cout << "Order " << jmsg[MESSAGE_REMOVE][MESSAGE_ORDER_ID] << " is successfully removed" << std::endl;
		}
		else {
			std::cout << "Order " << jmsg[MESSAGE_REMOVE][MESSAGE_ORDER_ID] << " removal is unsuccesfull" << std::endl;
		}
		cpen333::pause();
	}

	void do_search() {
		std::string orderID;

		// user input order id
		std::cout << "Search Order" << std::endl;
		std::cout << "Please enter order id: ";
		std::getline(std::cin, orderID);

		SearchMessage search(orderID, warehouse_id);
		api_.sendMessage(search);

		// wait for response
		std::unique_ptr<std::string> jsonStrPtr = api_.recvJSON();
		JSON jmsg = JSON::parse(*jsonStrPtr);

		//TODO: show response message
		std::cout << "Search order request response" << std::endl;
		if (jmsg[MESSAGE_STATUS] == MESSAGE_STATUS_OK) {
			std::cout << "Order " << jmsg[MESSAGE_SEARCH][MESSAGE_ORDER_ID] << " is successfully found" << std::endl;
		}
		else if (jmsg[MESSAGE_STATUS] == MESSAGE_STATUS_RECEIVED) {
			std::cout << "Order " << jmsg[MESSAGE_SEARCH][MESSAGE_ORDER_ID] << " is RECEIVED" << std::endl;
		}
		else if (jmsg[MESSAGE_STATUS] == MESSAGE_STATUS_PROCESSING) {
			std::cout << "We are PROCESSING order " << jmsg[MESSAGE_SEARCH][MESSAGE_ORDER_ID] << std::endl;
		}
		else if (jmsg[MESSAGE_STATUS] == MESSAGE_STATUS_SHIPPED) {
			std::cout << "Order " << jmsg[MESSAGE_SEARCH][MESSAGE_ORDER_ID] << " is SHIPPED" << std::endl;
		}
		else {
			std::cout << "Order " << jmsg[MESSAGE_SEARCH][MESSAGE_ORDER_ID] << " is NOT found" << std::endl;
		}
		cpen333::pause();
	}

	void do_remove_item() {
		std::string orderID;
		std::string itemName;

		std::cout << "Remove item from order" << std::endl;
		std::cout << "Enter order id: ";
		std::getline(std::cin, orderID);
		std::cout << "Enter item name: ";
		std::getline(std::cin, itemName);

		// create remove message and send message
		RemoveItemMessage remove_item(orderID, itemName, warehouse_id);
		api_.sendMessage(remove_item);

		// wait for response
		std::unique_ptr<std::string> jsonStrPtr = api_.recvJSON();
		JSON jmsg = JSON::parse(*jsonStrPtr);

		// show responses
		std::cout << "Search order request response" << std::endl;
		if (jmsg[MESSAGE_STATUS] == MESSAGE_STATUS_OK) {
			std::cout << "Item " << jmsg[MESSAGE_REMOVE_ITEM][MESSAGE_ITEM_NAME] << " is successfully removed" << std::endl;
		}
		else {
			std::cout << "Item " << jmsg[MESSAGE_REMOVE_ITEM][MESSAGE_ITEM_NAME] << " removal FAILED" << std::endl;
		}
		cpen333::pause();
	}

	void do_search_item() {
		std::string itemName;

		// user input
		std::cout << "Search for item" << std::endl;
		std::cout << "Enter item name: ";
		std::getline(std::cin, itemName);

		// create search item message and send message
		SearchItemMessage search_item(itemName, warehouse_id);
		api_.sendMessage(search_item);

		// wait for response
		std::unique_ptr<std::string> jsonStrPtr = api_.recvJSON();
		JSON jmsg = JSON::parse(*jsonStrPtr);

		// show responses
		std::cout << "Search order request response" << std::endl;
		if (jmsg[MESSAGE_STATUS] == MESSAGE_STATUS_OK) {
			std::cout << "Item " << jmsg[MESSAGE_SEARCH_ITEM][MESSAGE_ITEM_NAME] << " is successfully found" << std::endl;
		}
		else {
			std::cout << "Item " << jmsg[MESSAGE_SEARCH_ITEM][MESSAGE_ITEM_NAME] << " is NOT found" << std::endl;
		}
		cpen333::pause();
	}

	void do_goodbye() {
		GoodbyeMessage goodbye(warehouse_id);
		if (api_.sendMessage(goodbye)) {
			std::cout << "Goodbye!" << std::endl;
		}
	}

    /**
    * cancel an order, send to server
    * @param orderID ID of order
    */
    void do_cancel(std::string orderID) {}

    /**
     * connect to server at port
     * @return true if connected
     */
    bool connectServer() {
		if (!socket_.open())
			return false;
		else {
			std::cout << "Connected! \n" << std::endl;
			api_ = WarehouseComputerAPI(std::move(socket_));
		}
		return true;
	}

    /**
     * send quote to server
     */
    void sendQuote() {}


	void addQuote() {};

	void displayUI() {
		if (warehouse_id == NOT_SELECTED)
			selectWarehouse();

		std::cout << "=========================================" << std::endl;
		std::cout << "=                  MENU                 =" << std::endl;
		std::cout << "=========================================" << std::endl;
		std::cout << " (1) Add Quote" << std::endl;
		std::cout << " (2) Remove Order" << std::endl;
		std::cout << " (3) Search Order" << std::endl;
		std::cout << " (4) Remove Item" << std::endl;
		std::cout << " (5) Search Item" << std::endl;
		std::cout << " (6) Select Warehouse" << std::endl;
		std::cout << " (Q) Quit" << std::endl;
		std::cout << "=========================================" << std::endl;
		std::cout << "Enter number: ";
		std::cout.flush();
	}

	void selectWarehouse() {
		std::string int_str;

		std::cout << "=========================================" << std::endl;
		std::cout << "=          SELECT WAREHOSUE             =" << std::endl;
		std::cout << "=========================================" << std::endl;
		std::cout << " (0) Any Warehouse" << std::endl;
		std::cout << " (1) Warehouse 1" << std::endl;
		std::cout << " (2) Warehouse 2" << std::endl;
		std::cout << "=========================================" << std::endl;
		std::cout << "Enter number: ";
		std::cout.flush();

		std::getline(std::cin, warehouse_id);
	}
};

#endif //AMAZOOM_USERINTERFACE_H
