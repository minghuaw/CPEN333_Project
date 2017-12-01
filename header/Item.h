/**
 *@file
 *
 * This file contains the definition of an item in the database
 *
 */

#ifndef PROJECT_ITEM_H
#define PROJECT_ITEM_H

#include <string>
#include <iostream>
#include "Coordinate.h"

class Item {
private:
    std::string ID;                                // item ID
    double weight;                                // total weight of the item

    // TODO: how to construct the item????
    Item(const std::string ID, const double weight) :
            ID(ID), weight(weight) {}

    // TODO: what form?
    /**
     * Convert an item to string format
     * @return 	string in a form of ????
     */
public

    std::string toString() const {
    }


    /**
    * less-than operator for comparisons, sort by item ID
    * @param a	item to be compared
    * @param b	item to be compared
    * @return 	true if item a smaller than item b, false otherwise
    */
    friend bool operator<(const Item &a, const Item &b) {
        if (a.ID < b.ID) {
            return true;
        } else {
            return false;
        }
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

    /**
     * overloaded stream operator for printing, std::cout << Item
     * @return 	item in string format
     */
    friend std::ostream &operator<<(std::ostream &os, const Item &item) {
        os << item.toString();
        return os;
    }

};

class ItemInfo : public Item {
private:
    Coordinate location;
public:
    ItemInfo(std::string ID, const double weight, Coordinate location) : SuperClass(ID, weight) {}

    Coordinate getLocation() {}
};


#endif //PROJECT_ITEM_H
