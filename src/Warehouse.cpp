#include "Warehouse.h"

int main(){
	// create shared memory
	cpen333::process::shared_object<SharedData> memory(LAYOUT_MEMORY_NAME);
	cpen333::process::mutex mutex(LAYOUT_MEMORY_MUTEX_NAME);
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

	// keep reading commands until the user quits
	char cmd = 0;
	while (cmd != CLIENT_QUIT) {
		print_menu();

		// get menu entry
		std::cin >> cmd;
		// gobble newline
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (cmd) {
		case CLIENT_ADD:
			do_add(api);
			break;
		case CLIENT_REMOVE:
			do_remove(api);
			break;
		case CLIENT_SEARCH:
			do_search(api);
			break;
		case CLIENT_QUIT:
			do_goodbye(api);
			break;
		default:
			std::cout << "Invalid command number " << cmd << std::endl << std::endl;
		}

		cpen333::pause();
	}

	// wait for user to create robot
	// spawn robots
	std::cin.get();
	if (!warehouse.spawn_robot(1)) {
		std::cout << "error" << std::endl;
	}

	// wait for user to quit
	std::cout << "Press ENTER to quit" << std::endl;
	std::cin.get();
	std::cout << "Closing warehouse" << std::endl;

    return 0;
}
