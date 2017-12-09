//
// Warehouse.h
//

#ifndef AMAZOOM_WAREHOUSE_H
#define AMAZOOM_WAREHOUSE_H

#include "Amazoom.h"
#include <cpen333/process/semaphore.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cpen333/thread/semaphore.h>
#include "Layout.h"

class Warehouse{
private:
    cpen333::process::shared_object<SharedData> memory_;    // warehouse info
    cpen333::process::mutex mutex_;

    cpen333::thread::semaphore loadingBay;
    cpen333::thread::semaphore unloadingBay;
    Layout warehouseLayout;
    int magicKey;

public:
    /**
     * Warehouse constructor
     * Initialize magicKey to LAYOUT_MAGIC_KEY
     */
    Warehouse():memory_(LAYOUT_MEMORY_NAME),mutex_(LAYOUT_MEMORY_MUTEX_NAME),\
		loadingBay(LOADING_BAY_SEM_RESOURCE),unloadingBay(LOADING_BAY_SEM_RESOURCE),\
		magicKey(LAYOUT_MAGIC_KEY){}

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
