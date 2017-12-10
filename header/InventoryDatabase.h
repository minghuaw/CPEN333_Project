//
//  InventoryDatabase.h
//

#ifndef AMAZOOM_INVENTORYDATABASE_H
#define AMAZOOM_INVENTORYDATABASE_H

#define ITEM_NOT_FOUND -1

#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include "Coordinate.h"
#include "Item.h"
#include "Cell.h"
#include "Layout.h"

class InventoryDatabase{
private:
    //TODO: supply our own comparator for ItemInfo's, as map internally is a binary tree
    std::map<ItemInfo, int> inventory;						// map of iteminfo to quantity
	std::map<std::string, int> itemQuantity_;				// map of item name to quantity
	std::map<std::string, int> itemWeight_;					// map of item name to weight
	std::map<std::string, Coordinate> itemLocation_;
	//TODO: use map instead? to simplify the problem
    std::multimap<std::string, Coordinate> itemLocation;    // multimap of itemLocations. One type of item may be store
                                                            // multiple locations
    std::mutex mutex_;										// internal mutex for thread safe operation
	std::map<std::string, Cell> coordinate2cell;			// map coordinate to storage cells

	LayoutInfo linfo;										// layout information from text file

	std::vector<Cell> cells;								// list of cells
	std::vector<Coordinate> coors;							// list of coors

	InventoryDatabase();	// prevent default

public:
    /**
     * default constructor, creates an empty inventory
     */
	InventoryDatabase(LayoutInfo& linfo) : linfo(linfo) {
		initCoordinates();
		initCells();
	}

	/**
	* init a vector of coordinates objects based on layout info
	*/
	void initCoordinates() {
		for (int r = 0; r < linfo.rows; ++r) {
			for (int c = 0; c < linfo.cols; ++c) {
				char ch = linfo.layout[c][r];
				if (ch == SHELF_CHAR) {
					for (int level = 0; level < SHELF_LEVEL; level++) {
						char side = 'L';
						coors.push_back(Coordinate(c, r, side, level));
					}
					for (int level = 0; level < SHELF_LEVEL; level++) {
						char side = 'R';
						coors.push_back(Coordinate(c, r, side, level));
					}
				}
			}
		}
	}

	/**
	* init a vector of cells and map cells to coordinate
	*/
	void initCells() {
		for (auto coor : coors) {
			Cell c;
			cells.push_back(c);
			std::string coor_string = coor.toString();
			coordinate2cell.insert(std::pair<std::string,Cell>(coor_string,c));
		}
	}

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

	/**/
	ItemInfo toItemInfo(std::string itemName) {
		double weight = itemWeight_[itemName];
		Coordinate loc = itemLocation_[itemName];
		
		return ItemInfo(itemName, weight, loc);
	}

    /**
     * try to find item by its name in the inventory
     * @param itemName item name, std::string
     * @return true if item is found, false if item is not found
     */
    bool findItem(std::string itemName){
		if (itemQuantity_.find(itemName) == itemQuantity_.end())
			return false;
		if (itemWeight_.find(itemName) == itemWeight_.end())
			return false;
		if (itemLocation.find(itemName) == itemLocation.end())
			return false;

		return true;
	}

    /**
     * find the quantity of an item
     * @param itemName name of an item that is in the inventory
     * @return int quantity of the item, -1 if item not found
     */
    int findItemQuantity(std::string itemName){
		if (itemQuantity_.find(itemName) == itemQuantity_.end())
			return ITEM_NOT_FOUND;
		return itemQuantity_[itemName];
	}

	Coordinate& findItemLocation(std::string itemName) {
		return itemLocation_[itemName];
	}

    /**
     * find the weight of an item
     * @param itemName name of an item that is in the inventory
     * @return double weight of the item
     */
    double findItemWeight(std::string itemName){
		if (itemWeight_.find(itemName) == itemWeight_.end())
			return ITEM_NOT_FOUND;
		return itemWeight_[itemName];
	}

    /**
     * convert InventoryDatabase to a string
     * @return "InventoryDatabase"
     */
    std::string toString(){}

};
#endif //AMAZOOM_INVENTORYDATABASE_H
