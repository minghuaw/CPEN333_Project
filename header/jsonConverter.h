//
// Created by minghua on 09/12/17.
//

#ifndef AMAZOOM_JSONCONVERTER_H
#define AMAZOOM_JSONCONVERTER_H

#include <json.hpp>
#include <vector>
#include <memory>

// alias
using JSON = nlohmann::json;

// types of messages
#define MESSAGE_ADD "add"
#define MESSAGE_ADD_RESPONSE "add_response"
#define MESSAGE_REMOVE "remove"
#define MESSAGE_REMOVE_RESPONSE "remove_response"
#define MESSAGE_REMOVE_ITEM "removeItem"
#define MESSAGE_REMOVE_ITEM_RESPONSE "removeItem_response"
#define MESSAGE_SEARCH "search"
#define MESSAGE_SEARCH_RESPONSE "search_response"
#define MESSAGE_SEARCH_ITEM "searchItem"
#define MESSAGE_SEARCH_ITEM_RESPONSE "searchItem_response"
#define MESSAGE_GOODBYE "goodbye"

// other keys
#define MESSAGE_TYPE "msg"
#define MESSAGE_STATUS "status"
#define MESSAGE_ORDER "order"
#define MESSAGE_ADD_RESULTS "results"
#define MESSAGE_ITEM_QUANTITY "quantity"
#define MESSAGE_CLIENT_ID "clientID"
#define MESSAGE_ORDER_ID "orderID"
#define MESSAGE_ITEM_NAME "itemName"




#endif //AMAZOOM_JSONCONVERTER_H
