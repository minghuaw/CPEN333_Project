/**
 *@file
 *
 * This file contains the definition of an order in the database
 *
 */

#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <iostream>

class Order {
 private:
  std::string ID;								// order ID
  std::string status;							// status of order
  double weight;								// total weight of the order
  std::vector<std::pair<ItemInfo,int>> items;	// item info and quantity

  Order(std::string ID) :
      ID(ID) {}

  /**
   * return the status of an order
   * @return 	status of order in string format
   */
   public std::string returnOrderStatus() {
   }

   /**
   * change the status of an order
   * @param newStatus	status of an order to be changed
   */
   public changeOrderStatus(std::String newStatus) {
   }

   /**
   * return the ID of an order
   * @return 	ID of order in string format
   */
   public std::string returnOrderID() {
   }

   /**
   * assign truck ID to order
   * @return 	ID of order in string format
   */
   public assignTruckID(std::string truckID) {
   }

   /**
   * Convert an order to string format
   * @return 	order in string format
   */
  public std::string toString() const {
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
