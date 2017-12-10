//
// Quote.h
//

#ifndef AMAZOOM_QUOTE_H
#define AMAZOOM_QUOTE_H

#include <string>

class Quote {
private:
public:
    std::vector<std::pair<std::string,int>> quote_;

    /**
     * default constructor
     */
    Quote(){}

    /**
     * constructor
     * @param itemName single item name
     * @param quantity item wanted wanted
     */
    Quote(std::string itemName, int quantity){}

	/**
	* constructor
	* @param itemName single item name
	* @param quantity item wanted wanted
	*/
	Quote(std::vector<std::pair<std::string, int>>&& quote): quote_(quote) {}

    /**
     * constructor
     * @param itemVector vector of std::pair<std::string itemName, int quantity>
     */
    Quote(std::vector<std::pair<std::string, int >> &itemVector){}

    /**
     * add a quote of item to the quote
     * @param itemName name of item
     * @param quantity quantity of item
     */
    void addItemQuote(std::string itemName, int quantity){
		quote_.push_back(std::pair<std::string, int>(itemName, quantity));
	}

    /**
     * get an item quote from the quotes and remove pair from the vector from the back
     * @param outItemQuote  single item quote output, with item name and quantity
     * @return true if there are pairs in the vector
     */
    bool getItemQuote(std::pair<std::string,int>& outItemQuote) {}
};

#endif //AMAZOOM_QUOTE_H
