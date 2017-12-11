//
//  Item.h
//
#ifndef PROJECT_ITEM_H
#define PROJECT_ITEM_H

#include <string>
#include <iostream>
#include "Coordinate.h"

class Item {
protected:
    std::string ID;                      // item ID, 'ABC'
    double weight;                                // total weight of the item

public:
	/**
	 * default constructor
	 */
	Item() {}

    /**
	* constructor of item
	*/
    Item(std::string ID, const double weight) :weight(weight),ID(ID) {
	}

    /**
     * Convert an item to string format
     * @return 	string "[ItemName][ItemWeight]"
     */
    std::string toString() {
		std::string s;
		s += ID;
		s += std::to_string(weight);
		return s;
    }

	double getWeight() {
		return weight;
	}


    /**
    * less-than operator for comparisons, sort by item ID
    * @param a	item to be compared
    * @param b	item to be compared
    * @return 	true if item a smaller than item b, false otherwise
    */
    friend bool operator<(const Item &a, const Item &b) {
        return a.ID < b.ID;
    }

    /**
    * equal-to operator for comparisons, ID must match
    * @return 	true if item IDs are the same, false otherwise
    */
    friend bool operator==(const Item &a, const Item &b) {
        return (a.ID == b.ID);
    }

    /**
     * not-equal-to operator for comparisons
     * @return 	true if item IDs are not the same, false otherwise
     */
    friend bool operator!=(const Item &a, const Item &b) {
        return !(a == b);
    }

    ///**
    // * overloaded stream operator for printing, std::cout << Item
    // * @return 	item in string format
    // */
    //friend std::ostream &operator<<(std::ostream &os, const Item &item) {
    //    os << item.toString();
    //    return os;
    //}

};

class ItemInfo : public Item {
private:
    Coordinate location;
public:

	ItemInfo():Item() {}

	/**
	* constructor of item
	*/
	ItemInfo(std::string ID) :Item(ID,0){}

    /**
     * Constructor for ItemInfo class, calls Item constructor
     * @param ID item ID, string
     * @param weight weight of the item, double
     * @param location location of the item, Coordinate
     */
    ItemInfo(std::string ID, const double weight, Coordinate location) : Item(ID, weight), location(location) {}

    /**
     * returns the location of the item
     * @return Coordinate
     */
    Coordinate getLocation() {
		return location;
	}

    /**
     * set the location of the ItemInfo to loc
     * @param loc coordinate to set the ItemInfo location into
     */
    void setLocation(Coordinate loc){
		location = loc;
	}

	/**
	* return the ID of Item
	*/
	std::string getID() {
		return ID;
	}

	double getWeight() {
		return weight;
	}
};

#endif //PROJECT_ITEM_H
