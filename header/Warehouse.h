//
// Created by minghua on 30/11/17.
//

#ifndef AMAZOOM_WAREHOUSE_H
#define AMAZOOM_WAREHOUSE_H

#include "Amazoom.h"
#include <cpen333/process/semaphore.h>
#include "Layout.h"

class Warehouse{
private:
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
    void loadLayout(){}

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

    /**
     * start the Warehouse system
     * initialize all inter-thread communication queues
     * @return
     */
    int main(){}

};

#endif //AMAZOOM_WAREHOUSE_H
