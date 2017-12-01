//
//
//

#ifndef AMAZOOM_INVENTORYDATABASE_H
#define AMAZOOM_INVENTORYDATABASE_H

#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include "Coordinate.h"
#include "Item.h"

class InventoryDatabase{
private:
    std::map<ItemInfo, int> inventory;              // map of iteminfo to quantity
    std::multimap<std::string, Coordinate> itemLocation;    // multimap of itemLocations. One type of item may be store
                                                            // multiple locations
    std::mutex mutex_;              // internal mutex for thread safe operation
public:
    /**
     * default constructor, creates an empty inventory
     */
    InventoryDatabase(){}

    /**
     * TO-DO: Add more constructor
     */

    /**
     * add additional entry with quantity if ItemInfo is not in inventory
     * add up the quantity if the ItemInfo is found in the inventory
     * @param itemInfo ItemInfo object of the target item,
     * @param quantity quantity of the itemiInfo to add into inventory
     * @return true if addition is successful/warehouse not full, false if warehouse full
     */
    bool addItemInfo(ItemInfo& itemInfo, int quantity){}

    /**
     * reduce quantity of an ItemInfo
     * @param itemInfo ItemInfo object of the target item,
     * @param quantity quantity of the itemiInfo to add into inventory
     * @return true if reduction is successful, false if reduction is unsuccessful
     */
    bool reduceItemInfoQuantity(ItemInfo& itemInfo, int quantity){}

    /**
     * Assign a new location to an ItemInfo
     * @param itemInfo itemInfo ItemInfo object of the target item
     * @param loc Coordinate of the new location
     */
    void assignItemLocation(ItemInfo& itemInfo, Coordinate loc){}

    /**
     * remove the current location of the ItemInfos
     * @param itemInfo itemInfo ItemInfo object of the target item
     * @param loc Coordinate of the new location
     */
    void remvItemLocation(ItemInfo& itemInfo, Coordinate loc){}

    /**
     * change the current location of the ItemInfos
     * @param itemInfo itemInfo ItemInfo object of the target item
     * @param loc Coordinate of the new location
     */
    void changeItemLocation(ItemInfo& itemInfo, Coordinate loc){}

    /**
     * try to find item by its name in the inventory
     * @param itemName item name, std::string
     * @return true if item is found, false if item is not found
     */
    bool findItem(std::string itemName){}

    /**
     * find the quantity of an item
     * @param itemName name of an item that is in the inventory
     * @return int quantity of the item
     */
    int returnItemQuantity(std::string itemName){}

    /**
     * find the weight of an item
     * @param itemName name of an item that is in the inventory
     * @return double weight of the item
     */
    double findItemWeight(std::string itemName){}

    /**
     * convert InventoryDatabase to a string
     * @return "InventoryDatabase"
     */
    std::string toString(){}

};
#endif //AMAZOOM_INVENTORYDATABASE_H
