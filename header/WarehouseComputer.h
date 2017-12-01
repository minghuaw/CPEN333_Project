/**
 * @file 
 * This document defines the attribute and public operation of WarehouseComputer class
 * 
 */


#ifndef WHAREHOUSE_COMPUTER_H
#define WHAREHOUSE_COMPUTER_H

#include <string>
#include <iostream>

#include "InventoryDatabase.h"
#include "DynamicQueue.h"
#include "Layout.h"
#include "Coordinate.h"

class WarehouseComputer{
    private:
		InventoryDatabase database;
		OrderQueue robotOrderQueue;
		OrderQueue truckOrderQueue;
		ItemQueue loadingQueue;
		ItemQueue unloadingQueue;
		TruckQueue truckQueue;
		Layout warehouseLayout;
		std::mutex db_mutex_;			// synchronization of database
	
    public:
		/**
		 * Constructor for Coordinate class
		 */
		WarehouseComputer(){}
		
		/**
		 * return the coordinate and the corresponding quantity of item by finding item name. Executed after a quote is received.
		 * @param itemName	name of item to be found
		 * @param quantity	number of items needed
		 * @return a vector of coordinate and quantity pairs
		 */
		std::vector<std::pair<Coordinate, int>> findLocation(std::pair<std::string, int> itemQuan) {}

		/**
		 * confirm a restocking quote and convert it to an order, the central computer will then
		 * place the order in the truckOrderQueue, and put a void order in robotOrderQueue
		 * so that robots know to go to trucks
		 * @param quote		restocking quote to be confirmed by warehouse
		 * @param outorder	output order
		 * @return always return true
		 */
		bool confirmRestockingQuote(Quote quote, Order& outorder) {}
		
		/**
		 * confirm if a customer quote can be processed after checking stock, if confirmed, convert quote to an order
		 * and call "placeOrderInQueue" to put the order in the robotOrderQueue. This is being called from the socket 
		 * switch case after receving a quote from the RemoteWebServer
		 * @param quote		customer quote to be confirmed by warehouse
		 * @param outorder	output order
		 * @return true if a restocking quote can be confirmed, false otherwise
		 */
		bool confirmCustomerQuote(Quote quote, Order& outorder) {}
		
		/**
		 * place an order in the robotOrderQueue or truckOrderQueue depend on the order type
		 * @param order		order to be placed
		 */
		placeOrderInQueue(Order order) {}
		
		/**
		 * find the weight of an order
		 * @param order		order to be checked
		 * @return weight of order
		 */
		double findOrderWeight(Order& order) {}
		
		/**
		 * find the weight of an order
		 * @param order		order to be checked
		 * @return weight of order
		 */
		OrderStatus findOrderStatus(Order& order) {}
		
		/**
		 * remove an order only if the order is RECEIVED
		 * @param orderID	ID of an order
		 * @return true if order is removed, false otherwise
		 */
		bool removeOrder(Order& order) {}
		
		placeOrderInQueue() {}
};

#endif // WAREHOUSE_COMPUTER