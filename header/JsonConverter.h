//
// Created by minghua on 09/12/17.
//

#ifndef AMAZOOM_JSONCONVERTER_H
#define AMAZOOM_JSONCONVERTER_H

#include <json.hpp>
#include <vector>
#include <memory>
#include "Quote.h"
#include "Message.h"

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
#define MESSAGE_INFO "info"
#define MESSAGE_ORDER "order"
#define MESSAGE_RESULTS "results"
#define MESSAGE_ITEM_QUANTITY "quantity"
#define MESSAGE_CLIENT_ID "clientID"
#define MESSAGE_ORDER_ID "orderID"
#define MESSAGE_ITEM_NAME "itemName"
#define MESSAGE_ORDER_ITEM "item"

class JsonConverter
{
public:
	JsonConverter();
	~JsonConverter();

	/**
	 * convert pairs into json
	 * @param pair
	 * @return JSON entry in order list
	 */
	static JSON toJSON(const std::pair<std::string, int>& pair) {
		JSON j;
		j[MESSAGE_ITEM_NAME] = pair.first;
		j[MESSAGE_ITEM_QUANTITY] = pair.second;

		return j;
	}

	/**
	 * convert vector of pairs into a json list
	 * @param vect vector of pairs
	 * @return json list of json objects
	 */
	static JSON toJSON(const std::vector<std::pair<std::string, int>>& vect){
		JSON j;
		for (auto& pair : vect) {
			j.push_back(toJSON(pair));
		}
		return j;
	}

	/**
	 * convert quote object into a json list
	 * @param qt quote
	 * @return json list of json objects
	 */
	static JSON toJSON(const Quote qt) {
		return toJSON(qt.quote_);
	}

	/**
	 * convert an add message into a json message
	 * @param add add message
	 * @return json message
	 */
	static JSON toJSON(const AddMessage& add) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_ADD;
		//j[MESSAGE_CLIENT_ID] = 
		j[MESSAGE_ORDER] = toJSON(add.quote);
		return j;
	}

	/**
	* convert an add response message into a json message
	* @param add_resposne message
	* @return json message
	*/
	static JSON toJSON(const AddResponseMessage& add_response) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_ADD_RESPONSE;
		j[MESSAGE_ORDER_ID] = add_response.orderID;
		j[MESSAGE_CLIENT_ID] = add_response.clientID;
		j[MESSAGE_STATUS] = add_response.status;
		j[MESSAGE_INFO] = add_response.info;
		j[MESSAGE_ADD] = toJSON(add_response.add);

		return j;
	}

	/**
	* convert a remove message into a json message
	* @param remove message
	* @return json message
	*/
	static JSON toJSON(const RemoveMessage& remove) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_REMOVE;
		j[MESSAGE_ORDER_ID] = remove.orderID;
		return j;
	}

	/**
	* convert a remove response message into a json message
	* @param remove_response message
	* @return json message
	*/
	static JSON toJSON(const RemoveResponseMessage& remove_response) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_REMOVE_RESPONSE;
		j[MESSAGE_CLIENT_ID] = remove_response.clientID;
		j[MESSAGE_STATUS] = remove_response.status;
		j[MESSAGE_INFO] = remove_response.info;
		j[MESSAGE_REMOVE] = toJSON(remove_response.remove);

		return j;
	}

	/**
	* convert a remove item message into a json message
	* @param removeItem message
	* @return json message
	*/
	static JSON toJSON(const RemoveItemMessage& removeItem) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_REMOVE_ITEM;
		j[MESSAGE_ORDER_ID] = removeItem.orderID;
		j[MESSAGE_ITEM_NAME] = removeItem.itemName;
		return j;
	}

	/**
	* convert a remove item response message into a json message
	* @param removeItem_response message
	* @return json message
	*/
	static JSON toJSON(const RemoveItemResponseMessage& removeItem_response) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_REMOVE_ITEM_RESPONSE;
		j[MESSAGE_CLIENT_ID] = removeItem_response.clientID;
		j[MESSAGE_STATUS] = removeItem_response.status;
		j[MESSAGE_INFO] = removeItem_response.info;
		j[MESSAGE_REMOVE_ITEM] = toJSON(removeItem_response.removeItem);

		return j;
	}

	/**
	* convert a search message into a json message
	* @param search message
	* @return json message
	*/
	static JSON toJSON(const SearchMessage& search) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_SEARCH;
		j[MESSAGE_ORDER_ID] = search.orderID;
		return j;
	}

	static JSON toJSON(const SearchResponseMessage& search_response) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_SEARCH_RESPONSE;
		j[MESSAGE_CLIENT_ID] = search_response.clientID;
		j[MESSAGE_STATUS] = search_response.status;
		j[MESSAGE_INFO] = search_response.info;
		j[MESSAGE_SEARCH] = toJSON(search_response.search);

		return j;
	}

	/**
	* convert a search item message into a json message
	* @param searchItem message
	* @return json message
	*/
	static JSON toJSON(const SearchItemMessage& searchItem) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_SEARCH_ITEM;
		j[MESSAGE_ITEM_NAME] = searchItem.itemName;
		return j;
	}

	/**
	* convert a search item response message into a json message
	* @param searchItem message
	* @return json message
	*/
	static JSON toJSON(const SearchItemResponseMessage& searchItem_response) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_SEARCH_ITEM_RESPONSE;
		j[MESSAGE_CLIENT_ID] = searchItem_response.clientID;
		j[MESSAGE_STATUS] = searchItem_response.status;
		j[MESSAGE_INFO] = searchItem_response.info;
		j[MESSAGE_SEARCH_ITEM] = toJSON(searchItem_response.searchItem);

		return j;
	}

	static JSON toJSON(const GoodbyeMessage& goodbye) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_GOODBYE;
		return j;
	}

	/**
	 * convert message into json message according to the message type
	 * @param msg
	 * @return
	 */
	static JSON toJSON(const Message& msg) {
		switch (msg.type())
		{
		case ADD:
			return toJSON((AddMessage &)msg);
			break;
		case ADD_RESPONSE:
			return toJSON((AddResponseMessage &)msg);
			break;
		case REMOVE:
			return toJSON((RemoveMessage &)msg);
			break;
		case REMOVE_RESPONSE:
			return toJSON((RemoveResponseMessage &)msg);
			break;
		case REMOVE_ITEM:
			return toJSON((RemoveItemMessage &)msg);
			break;
		case REMOVE_ITEM_RESPONSE:
			return toJSON((RemoveItemResponseMessage &)msg);
			break;
		case SEARCH:
			return toJSON((SearchMessage &)msg);
			break;
		case SEARCH_RESPONSE:
			return toJSON((SearchResponseMessage &)msg);
			break;
		case SEARCH_ITEM:
			return toJSON((SearchItemMessage &)msg);
			break;
		case SEARCH_ITEM_RESPONSE:
			return toJSON((SearchItemResponseMessage &)msg);
			break;
		case GOODBYE:
			return toJSON((GoodbyeMessage &)msg);
			break;
		default:
			std::cout << "Unknown MessageType" << std::endl;
			return JSON();
			break;
		}
	}

	static std::pair<std::string, int> parsePair(const JSON& jmsg) {
		std::string itemName = jmsg[MESSAGE_ITEM_NAME];
		int num = jmsg[MESSAGE_ITEM_QUANTITY];

		return std::pair< std::string, int>(itemName, num);
	}

	static std::vector<std::pair<std::string, int>> parseVector(JSON& jmsg) {
		std::vector<std::pair<std::string, int>> items;

		for (JSON& j_item : jmsg) {
			items.push_back(parsePair(j_item));
		}
		
		return items;
	}

	/**/
	static AddMessage parseAdd(JSON& jmsg) {
		std::cout << "client id" << jmsg[MESSAGE_CLIENT_ID] << std::endl;
		int cliendID = jmsg[MESSAGE_CLIENT_ID];
		Quote quote(parseVector(jmsg[MESSAGE_ORDER]));
		AddMessage add(quote, cliendID);
		
		return add;
	}

	/**
	* Detects the message type from a JSON object
	* @param jmsg JSON object
	* @return message type
	*/
	static MessageType parseType(const JSON& jmsg) {
		std::string type = jmsg[MESSAGE_TYPE];

		if (type == MESSAGE_ADD) { return MessageType::ADD; }
		else if (type == MESSAGE_ADD_RESPONSE) { return MessageType::ADD_RESPONSE; }
		else if (type == MESSAGE_REMOVE) { return MessageType::REMOVE; }
		else if (type == MESSAGE_REMOVE_RESPONSE) { return MessageType::REMOVE_RESPONSE; }
		else if (type == MESSAGE_SEARCH) { return MessageType::SEARCH; }
		else if (type == MESSAGE_SEARCH_RESPONSE) { return MessageType::SEARCH_RESPONSE; }
		else if (type == MESSAGE_REMOVE_ITEM) { return MessageType::REMOVE_ITEM; }
		else if (type == MESSAGE_REMOVE_ITEM_RESPONSE) { return MessageType::REMOVE_ITEM_RESPONSE; }
		else if (type == MESSAGE_SEARCH_ITEM) { return MessageType::SEARCH_ITEM; }
		else if (type == MESSAGE_SEARCH_ITEM_RESPONSE) { return MessageType::SEARCH_ITEM_RESPONSE; }
		else { return MessageType::GOODBYE; }
	}

	static std::unique_ptr<Message> parseMessage(JSON& jmsg) {
		std::cout << "[parser]" << jmsg.dump() << std::endl;
		MessageType type = parseType(jmsg);

		switch (type)
		{
		case ADD:
			return std::unique_ptr<Message>(new AddMessage(parseAdd(jmsg)));
			break;
		case ADD_RESPONSE:
			break;
		case REMOVE:
			break;
		case REMOVE_RESPONSE:
			break;
		case REMOVE_ITEM:
			break;
		case REMOVE_ITEM_RESPONSE:
			break;
		case SEARCH:
			break;
		case SEARCH_RESPONSE:
			break;
		case SEARCH_ITEM:
			break;
		case SEARCH_ITEM_RESPONSE:
			break;
		case GOODBYE:
			break;
		default:
			break;
		}
	}

	static std::unique_ptr<Message> parseMessage(const std::string& jstr) {
		JSON j;
		j = JSON::parse(jstr);
		return parseMessage(j);
	}

private:

};

JsonConverter::JsonConverter()
{
}

JsonConverter::~JsonConverter()
{
}


#endif //AMAZOOM_JSONCONVERTER_H
