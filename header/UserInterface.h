//
// Created by mli on 12/1/2017.
//

#ifndef AMAZOOM_USERINTERFACE_H
#define AMAZOOM_USERINTERFACE_H

#include "Order.h"
#include <vector>
#include <string>
#include "Quote.h"

class UserInterface{
private:
    Quote selectedItems;
public:
    /**
     * default constructor
     */
    UserInterface(){}

    /**
     * query the status of an order
     * for manager, check shared object directly, for client, send to server
     * @param orderID   the ID of order
     * @return order status
     */
    virtual OrderStatus queryOrder(std::string orderID){}

    /**
     * add a quote of selected items, clear selected items after sending
     * for manager, push to queue, for client, send to server
     */
    virtual addQuote(){}

    /**
     * select an item to add to selected items
     * @param itemName  the name of item
     * @param quantity  the quantity of item
     */
    void selectItem(std::string itemName, int quantity){}

    /**
     * display options, constantly called within a while loop
     */
    void displayUI() {}
};

class ManagerUI: public UserInterface {
private:
    cpen333::process::shared_object<SharedData> memory_;

public:
    /**
     * default constructor, calls default constructor of superclass
     */
    ManagerUI():UserInterface(){}

    /**
    * cancel an order, if not processing or shipped
    * @param orderID ID of order
    * @return true if order is cancelled
    */
    bool cancelOrder(std::string orderID) {}

    /**
     * check stock of items in warehouse
     * @param itemName
     * @return quantity of itemName, return NAN if item not existed
     */
    int checkStock(std::string itemName) {}

    /**
     * print the name of low stock item on screen
     * @param itemName  name of item low in stock
     * @param quantity quantity of item
     */
    void alertLowStock(std::string itemName, int quantity) {}

    /**
     * create another thread for delivery truck
     */
    void addDeliveryTruck() {}

    /**
     * create another thread for restock truck
     */
    void addRestockTruck() {}

    /**
     * create another thread for robot, modify shared variable
     */
    void addRobot() {}

    /**
     * remove a robot, join thread and modify shared variable
     * @param robotID id of robot to be removed
     */
    void removeRobot(std::string robotID) {}

    /**
     * push quote in quoteQueue
     */
    void sendQuote() {}
};

class ClientUI: UserInterface {
private:
    pen333::process::socket socket;
public:
    /**
     * default constructor
     * calls default constructor of superclass
     */
    ClientUI():UserInterface(){}
    /**
    * cancel an order, send to server
    * @param orderID ID of order
    */
    void do_cancel(std::string orderID) {}

    /**
     * connect to server at port
     * @param port port number of server
     * @return true if connected
     */
    bool connectServer(int port) {}

    /**
     * send quote to server
     */
    void sendQuote() {}
};

#endif //AMAZOOM_USERINTERFACE_H
