/**
 *@file Order.h
 *
 * This file contains the definition of an order in the database
 *
 */

#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <iostream>
#include <vector>
#include "Item.h"

enum OrderType {
  CLIENT,
  MANAGER,
  UNKNOWN
};

enum OrderStatus {
  RECEIVED,		// when computer creates the order
  PROCESSING,	// robot starts to pick it up and put on truck
  SHIPPED		// robot finishes the order
};

class Order {
 private:
  std::string ID;								// order ID
  OrderStatus status;							// status of order
  double weight;								// total weight of the order
  OrderType type;								// type of order, either from client or manager
  std::vector<std::pair<ItemInfo,int>> items;	// item info and quantity
  //std::string truckID;

public:
    /**
     * default constructor
     */
    Order(){}

    /**
     *
     * @param ID order id in string format
     */
    Order(std::string ID) : ID(ID) {}

	Order(std::string ID, std::vector<std::pair<ItemInfo, int>> items, OrderType type=CLIENT): 
		ID(ID), items(items), type(type){
		status = RECEIVED;
	}

    /**
    * return the status of an order
    * @return 	status of order
    */
    OrderStatus returnOrderStatus() {
    }
   
    /**
    * change the status of an order
    * @param newStatus	status of an order to be changed
    */
    void changeOrderStatus(OrderStatus newStatus) {
    }
   
    /**
    * return the ID of an order
    * @return 	ID of order in string format
    */
    std::string returnOrderID() {
		return ID;
    }

	/**
	* return order type
	*/
	OrderType returnOrderType() {
		return type;
	}
   
    ///**
    //* assign truck ID to order
    //* @return 	ID of order in string format
    //*/
    //void assignTruckID(std::string truckID) {
    //}

    /**
    * Convert an order to string format
    * @return 	order in string format
    */
    std::string toString() const {
    }

    /**
     * get an item info from the order and remove pair from the vector from the back
     * @param outItemInfo  output with item info and quantity
     * @return true if there are still pairs in the vector
     */
    bool getItemInfo(std::pair<ItemInfo,int>& outItemInfo) {
		if (items.size() > 0) {
			outItemInfo = items.back();
			items.pop_back();
			return true;
		}
		else
			return false;
	}

	bool removeItem(std::string itemName) {
		int size = items.size();
		std::string name;

		for (int i = 0; i < size; i++) {
			name = items[i].first.getID();

			if (name == itemName) {
				items.erase(items.begin()+i);
				return true;
			}
		}
		return false;
	}
   

    /**
    * less-than operator for comparisons, sort by order ID
    * @param a	order to be compared
    * @param b	order to be compared
    * @return 	true if order a smaller than order b, false otherwise
    */
    friend bool operator<(const Order& a, const Order& b) {
    if (a.ID < b.ID) {
        return true;
    }
    else {
        return false;
    }
    }

   /**
   * equal-to operator for comparisons, ID must match 
   * @return 	true if order IDs are the same, false otherwise
   */
  friend bool operator==(const Order& a, const Order& b) {
    return (a.ID == b.ID);
  }

  /**
   * not-equal-to operator for comparisons
   * @return 	true if order IDs are not the same, false otherwise
   */
  friend bool operator!=(const Order& a, const Order& b) {
    return !(a == b);
  }

  /**
   * overloaded stream operator for printing, std::cout << Order
   * @return 	order in string format
   */
  friend std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.toString();
    return os;
  }
};

#endif //ORDER_H
