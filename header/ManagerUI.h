#ifndef AMAZOOM_MANAGERUI_H
#define AMAZOOM_MANAGERUI_H

#define MANAGER_QUIT 'Q'

#define ADD_ROBOT 'R'
#define REMOVE_ROBOT 'T'

#define ADD_DELIVERY_TRUCK 'D'
#define ADD_RESTOCK_TRUCK 'S'

#define ADD_ORDER 'A'
#define ADD_ITEM 'A'
#define CONFIRM_ORDER 'C'

/**
* main menu for manager, called by showManagerUI()
*/
void print_mainmenu() {

	std::cout << "=========================================" << std::endl;
	std::cout << "=              MAIN  MENU               =" << std::endl;
	std::cout << "=========================================" << std::endl;
	std::cout << " (R) Add Robot" << std::endl;
	std::cout << " (T) Remove Robot" << std::endl;
	std::cout << " (D) Add Delivery Truck" << std::endl;
	std::cout << " (S) Add Restock Truck" << std::endl;
	std::cout << " (A) Start Restock Order" << std::endl;
	std::cout << " (Q) Quit" << std::endl;
	std::cout << "=========================================" << std::endl;
	std::cout << "Enter Letter: ";
	std::cout.flush();
}

/**
* order menu for manager, called by showManagerUI()
*/
void print_ordermenu() {
	std::cout << "=========================================" << std::endl;
	std::cout << "=            ORDER MENU                 =" << std::endl;
	std::cout << "=========================================" << std::endl;
	std::cout << " (A) Add Item" << std::endl;
	std::cout << " (C) Place Order" << std::endl;
	std::cout << " (Q) Cancel" << std::endl;
	std::cout << "=========================================" << std::endl;
	std::cout << "Enter Letter: ";
	std::cout.flush();
}

#endif //AMAZOOM_MANAGERUI_H