#include "Warehouse.h"
#include "WarehouseComputerAPI.h"
#include "Message.h"
#include "Amazoom.h"
#include <string>
#include <deque>
#include "JsonConverter.h"
#include "json.hpp"
#include "DynamicQueue.h"

static const char CLIENT_ADD = '1';
static const char CLIENT_REMOVE = '2';
static const char CLIENT_SEARCH = '3';
static const char CLIENT_REMOVE_ITEM = '4';
static const char CLIENT_SEARCH_ITEM = '5';
static const char CLIENT_QUIT = 'Q';

int orderCounter = 1;

void service(WarehouseComputerAPI&& api_, OrderQueue& queue_, InventoryDatabase& inventory_){
	std::unique_ptr<Message> msg = api_.recvMessage();

	//std::unique_ptr<std::string> msg = api_.recvJSON();
	//std::cout << "[warehouse]" << *msg << std::endl;
	//JSON jmsg = JSON::parse(*msg);
	//std::cout << "[warehouse] string msg parsed to json" << std::endl;
	//std::unique_ptr<Message> mptr = JsonConverter::parseMessage(jmsg);
	//std::cout << mptr->type() << std::endl;

	while (msg != nullptr) {
		MessageType type = msg->type();
		switch (type)
		{
		case MessageType::ADD: {
			// cast message to add type
			AddMessage& add = (AddMessage &)(*msg);
			// extract quote out of add message
			Quote quote = add.quote;
			Order od;
			int clientID = add.clientID; 
			std::string orderID;

			bool success;
			std::string itemName;
			int num;

			// find if quote can be accomplished			
			success = Warehouse::confirmClientQuote(std::ref(quote), std::ref(inventory_),
				std::ref(orderCounter), std::ref(od));

			// send response message
			if (success) {
				//orderID = "C0" + std::to_string(orderCounter);
				AddResponseMessage add_re(add, od.returnOrderID(), clientID, MESSAGE_STATUS_OK);
				api_.sendMessage(add_re); // send back response message
				orderCounter++; // increase order counter for orderID

				// create client order
			/*	Order order = Warehouse::toOrder(std::ref(orderID), std::ref(quote), 
							std::ref(inventory_), OrderType::CLIENT);	*/	

				// push client order to order queue
				queue_.addOrder(std::ref(od));
				std::cout << "New order added to orderQueue" << std::endl;
			}
			else {
				orderID = "F00";
				AddResponseMessage add_re(add, orderID, clientID, MESSAGE_STATUS_ERROR);
				api_.sendMessage(add_re);
			}
			break;		
		}
		case MessageType::REMOVE: {
			RemoveMessage& remove = (RemoveMessage &)(*msg);

			// get order id
			std::string orderID = remove.orderID;

			// get client id
			int clientID = remove.clientID;
			std::cout << "Client " << clientID << " removing order " << orderID << std::endl;

			// try to remove the orderID
			bool success = queue_.removeOrder(std::ref(orderID));
			if (success) {
				std::cout << "Order " << orderID << " cancelled succesfully" << std::endl;
				RemoveResponseMessage remove_re(std::ref(remove), clientID, MESSAGE_STATUS_OK);
				api_.sendMessage(remove_re);
			}
			else {
				std::cout << "Order " << orderID << " cancellation failed" << std::endl;
				RemoveResponseMessage remove_re(std::ref(remove), clientID, MESSAGE_STATUS_ERROR);
				api_.sendMessage(remove_re);
			}
			break;		
		}
		case MessageType::REMOVE_ITEM: {
			RemoveItemMessage& remove_item = (RemoveItemMessage &)(*msg);

			std::string orderID = remove_item.orderID;
			std::string itemName = remove_item.itemName;
			int clientID = remove_item.clientID;

			bool success;
			Order od;
			// try to find the order
			success = queue_.searchOrderID(orderID, std::ref(od));

			if (!success) {
				// order does not exist
				std::cout << "Order " << orderID << " not found" << std::endl;
				RemoveItemResponseMessage remove_item_re(remove_item, std::ref(clientID), MESSAGE_STATUS_ERROR);
				api_.sendMessage(remove_item_re);
			}
			else {
				// order found
				std::cout << "Order " << orderID << " found" << std::endl;
				// try to remove item from order
				success &= od.removeItem(itemName);

				if (success) {
					//item removed
					std::cout << "Item " << itemName << " removed" << std::endl;
					RemoveItemResponseMessage remove_item_re(remove_item, std::ref(clientID), MESSAGE_STATUS_OK);
					api_.sendMessage(remove_item_re);
				}
				else {
					std::cout << "Item " << itemName << " NOT removed" << std::endl;
					RemoveItemResponseMessage remove_item_re(remove_item, std::ref(clientID), MESSAGE_STATUS_ERROR);
					api_.sendMessage(remove_item_re);
				}
			}
			break;
		}
		case MessageType::SEARCH: {
			SearchMessage& search = (SearchMessage &)(*msg);
			std::string orderID = search.orderID;
			int clientID = search.clientID;
			std::cout << "Client " << clientID << " searching order " << orderID << std::endl;

			// search order
			Order outorder;
			bool success = queue_.searchOrderID(orderID, std::ref(outorder));
			if (success) {
				std::cout << "Order " << orderID << " found" << std::endl;
			/*	SearchResponseMessage search_re(search, std::ref(clientID), MESSAGE_STATUS_OK);
				api_.sendMessage(search_re);*/

				switch (outorder.returnOrderStatus())
				{
				case OrderStatus::PROCESSING: {
					SearchResponseMessage search_re(search, std::ref(clientID), MESSAGE_STATUS_PROCESSING);
					api_.sendMessage(search_re);
					break;
				}
				case OrderStatus::RECEIVED: {
					SearchResponseMessage search_re(search, std::ref(clientID), MESSAGE_STATUS_RECEIVED);
					api_.sendMessage(search_re);
					break;
				}
				case OrderStatus::SHIPPED: {
					SearchResponseMessage search_re(search, std::ref(clientID), MESSAGE_STATUS_SHIPPED);
					api_.sendMessage(search_re);
					break;
				}
				default: {
					SearchResponseMessage search_re(search, std::ref(clientID), MESSAGE_STATUS_OK);
					api_.sendMessage(search_re);
					break;
				}
				}
			}
			else {
				std::cout << "Order " << orderID << " NOT found" << std::endl;
				SearchResponseMessage search_re(search, std::ref(clientID), MESSAGE_STATUS_ERROR);
				api_.sendMessage(search_re);
			}
			break;
		}
		case MessageType::SEARCH_ITEM: {
			SearchItemMessage& search_item = (SearchItemMessage &)(*msg);
			std::string itemName = search_item.itemName;
			int clientID = search_item.clientID;

			std::cout << "Client " << clientID << " searching for " << itemName << std::endl;

			bool success = inventory_.findItem(itemName);
			if (success) {
				std::cout << "Item " << itemName << " found" << std::endl;
				SearchItemResponseMessage search_item_re(search_item, std::ref(clientID), MESSAGE_STATUS_OK);
				api_.sendMessage(search_item_re);
			}
			else {
				std::cout << "Item " << itemName << " NOT found" << std::endl;
				SearchItemResponseMessage search_item_re(search_item, std::ref(clientID), MESSAGE_STATUS_ERROR);
				api_.sendMessage(search_item_re);
			}
			break;
		}
		case MessageType::GOODBYE: {
			GoodbyeMessage& goodbye = (GoodbyeMessage &)(*msg);
			std::cout << "Client " << goodbye.clientID << " left" << std::endl;
			break;
		}
		default: {
			break;
		}
		}

		//msg = api_.recvJSON();
		msg = api_.recvMessage();
	}
}

void connectToServer(OrderQueue& queue, InventoryDatabase& inventory,
	std::string host = LOCAL_HOST, int port = WAREHOUSE_PORT) {
	WarehouseComputerAPI api;

	cpen333::process::socket socket_(host, port);
	std::cout << "Warehouse socket initilizaed" << std::endl;
	std::cout << "Connecting to remote webserver...\n" << std::endl;
	std::cout.flush();

	// connecting socket
	if (socket_.open()) {
		std::cout << "Warehosue Connected!! \n" << std::endl;
		api = WarehouseComputerAPI(std::move(socket_));

		// enter service loop
		//service(std::move(api), std::ref(queue));
		auto service_thread = std::thread(service, std::move(api), std::ref(queue),std::ref(inventory));
		service_thread.detach();
	}
}

int main(){
	// create shared memory
	cpen333::process::shared_object<SharedData> memory(WAREHOUSE_MEMORY_NAME);
	cpen333::process::mutex mutex(WAREHOUSE_MEMORY_MUTEX_NAME);
	LayoutInfo linfo;

	// initialize shared memory
	{
		std::unique_lock<decltype(mutex)> lock(mutex);
		memory->quit = false;
		// create magic number
		memory->magic = LAYOUT_MAGIC_KEY;
		// load layout
		std::string layout = "../data/layout0.txt";
		loadLayoutInfo(layout, memory->linfo);
		linfo = memory->linfo;
	}

	// init database
	InventoryDatabase inventory(linfo);
	// initialize warehouse
	std::cout << "Starting Warehouse" << std::endl;
	Warehouse warehouse(std::ref(inventory));

	// connect to server
	/*OrderQueue queue;
	connectToServer(std::ref(queue), std::ref(inventory));*/
	connectToServer(std::ref(warehouse.robotOrderQueue), std::ref(inventory));

	std::cout << "========================================" << std::endl;
	cpen333::pause();

	// show manager UI
	warehouse.showManagerUI();

	//quit
	warehouse.close();

    return 0;
}
