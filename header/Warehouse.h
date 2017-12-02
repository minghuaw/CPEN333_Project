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
