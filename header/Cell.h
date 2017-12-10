#ifndef CELL_H
#define CELL_H

#include "Item.h"

class Cell
{
private:
	double capacity; //capacity of the cell
	double currWeight;   // current weight of all items in the cell
	std::map<std::string, int> item2quantity;			// map item name to quantity

public:
	/**
	* constructor for cell
	*/
	Cell(): capacity(CELL_CAPACITY),currWeight(0) {};

	/**
	* return the current weight of cell
	* @return cell weight
	*/
	double getCurrentWeight() {
		return currWeight;
	}

	// TODO: add protection to the following functions??
	/**
	* add item to cell
	* @param item	item to be added
	* @param quantity # of items to be added
	*/
	void addItem(Item& item, int quantity) {
		item2quantity.insert(std::pair<std::string,int>(item.toString(), quantity));
	}

	/**
	* remove item from cell
	* @param quantity # of items to be removed
	* @return item	item to removed
	*/
	Item removeItem(std::string itemname, int quantity) {
		auto result = item2quantity.find(itemname);
		result->second -= quantity;
	}
};
#endif // CELL_H