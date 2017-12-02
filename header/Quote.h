//
// Created by mli on 12/1/2017.
//

#ifndef AMAZOOM_QUOTE_H
#define AMAZOOM_QUOTE_H

#include <string>

class Quote {
private:
    std::vector<std::pair<std::string,int>> quotes;
public:
    /**
     * add a quote of item to the quote
     * @param itemName name of item
     * @param quantity quantity of item
     */
    void addItemQuote(std::string itemName, int quantity){}

    /**
     * get an item quote from the quotes and remove pair from the vector from the back
     * @param outItemQuote  single item quote output, with item name and quantity
     * @return true if there are pairs in the vector
     */
    bool getItemQuote(std::pair<std::string,int>& outItemQuote) {}
};

#endif //AMAZOOM_QUOTE_H
