//
// Created by minghua on 30/11/17.
//

#include <iostream>
#include "Warehouse.h"

/**
  * start the Warehouse system
  * initialize all inter-thread communication queues
  */
int main(){
	std::cout << "Starting Warehouse" << std::endl;
	Warehouse warehouse;
	std::cin.get();
	std::cout << "Press ENTER to quit" << std::endl;
	std::cout << "Closing warehouse" << std::endl;
    return 0;
}
