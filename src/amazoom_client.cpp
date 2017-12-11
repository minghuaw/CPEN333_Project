//
// client
//

#include <iostream>
#include "WarehouseComputerAPI.h"
#include "UserInterface.h"

static const char CLIENT_ADD = '1';
static const char CLIENT_REMOVE = '2';
static const char CLIENT_SEARCH = '3';
static const char CLIENT_REMOVE_ITEM = '4';
static const char CLIENT_SEARCH_ITEM = '5';
static const char CLIENT_SELECT_WAREHOUSE = '6';
static const char CLIENT_QUIT = 'Q';

int main(){
	ClientUI client;
	
	if (client.connectServer()) {
		char cmd = 0;
		while (cmd != CLIENT_QUIT) {
			client.displayUI();

			// get UI entry
			std::cin >> cmd;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			
			switch (cmd){
			case CLIENT_ADD:
				client.do_add();
				break;
			case CLIENT_REMOVE:
				client.do_remove();
				break;
			case CLIENT_SEARCH:
				client.do_search();
				break;
			case CLIENT_REMOVE_ITEM:
				client.do_remove_item();
				break;
			case CLIENT_SEARCH_ITEM:
				client.do_search_item();
				break;
			case CLIENT_SELECT_WAREHOUSE:
				client.selectWarehouse();
				break;
			case CLIENT_QUIT:
				client.do_goodbye();
				break;
			default:
				std::cout << "Invalid command " << cmd << std::endl << std::endl;
			}
		}
		
	}
	cpen333::pause();
    return 0;
}
