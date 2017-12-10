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
			// testing
			std::cout << "[special check] "<< "client ID " << add.clientID << std::endl;
			// extract quote out of add message
			Quote quote = add.quote;
			int clientID = add.clientID; 
			std::string orderID;

			bool success = true;
			std::string itemName;
			int num;
			// find if quote can be accomplished
			for (auto& pair : quote.quote_) {
				itemName = pair.first;
				num = pair.second;

				// find item
				if (inventory_.findItem(itemName)) {
					success &= true;
				}
				else {
					success &= false;
				}
			}

			// send response message
			if (success) {
				orderID = "C0" + std::to_string(orderCounter);
				AddResponseMessage add_re(add, orderID, clientID, MESSAGE_STATUS_OK);
				api_.sendMessage(add_re); // send back response message
				orderCounter++; // increase order counter for orderID

				// create client order
				Order order = Warehouse::toOrder(std::ref(orderID), std::ref(quote), 
							std::ref(inventory_), OrderType::CLIENT);		

				// push client order to order queue
				queue_.addOrder(std::ref(order));
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
			std::string orderID = remove.orderID;
			int clientID = remove.clientID;
			std::cout << "Client " << clientID << " removing order " << orderID << std::endl;


			break;		
		}
		case MessageType::REMOVE_ITEM: {
			break;
		}
		case MessageType::SEARCH: {
			break;
		}
		case MessageType::SEARCH_ITEM: {
			break;
		}
		case MessageType::GOODBYE: {
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
	Warehouse warehouse(inventory);

	// connect to server
	OrderQueue queue;
	connectToServer(std::ref(queue), std::ref(inventory));

	std::cout << "========================================" << std::endl;
	cpen333::pause();

	// show manager UI
	warehouse.showManagerUI();

	//quit
	warehouse.close();

    return 0;
}
