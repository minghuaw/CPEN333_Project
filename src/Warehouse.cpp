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

	// show manager UI
	warehouse.showManagerUI();

	//quit
	warehouse.close();

    return 0;
}
