//
// User Interface
//

#ifndef AMAZOOM_USERINTERFACE_H
#define AMAZOOM_USERINTERFACE_H

#include "Order.h"
#include <vector>
#include <string>
#include "Quote.h"

class UserInterface{
private:

public:
    /**
     * default constructor
     */
    UserInterface(){}

    OrderStatus queryOrder(std::string){}

    std::vector<std::pair<std::string, int>> selectItems(){}

    Quote createQuote(std::vector<std::pair<std::string, int>>){}

    void displayUI(){}

};

class ManagerUI: public UserInterface{
private:
public:
};

#endif //AMAZOOM_USERINTERFACE_H
