/**
 * @file Message.h
 *
 * This file contains all message-related objects, independent of the specific API
 *
 * This middle layer allows us to abstract away many of the communication details,
 * allowing us to focus on the core functional implementation.
 *
 */
#ifndef PROJECT_MESSAGES_H
#define PROJECT_MESSAGES_H

#include "Order.h"
#include "Quote.h"
#include <string>
#include <vector>

/**
 * Types of messages that can be sent between client/server
 */
enum MessageType {
    ADD,
    ADD_RESPONSE,
    REMOVE,
    REMOVE_RESPONSE,
    REMOVE_ITEM,
    REMOVE_ITEM_RESPONSE,
    SEARCH,
    SEARCH_RESPONSE,
    SEARCH_ITEM,
    SEARCH_ITEM_RESPONSE,
    GOODBYE,
    //UNKNOWN,
};

// status messages for response objects
#define MESSAGE_STATUS_OK "OK"
#define MESSAGE_STATUS_ERROR "ERROR"
#define MESSAGE_STATUS_RECEIVED "RECEIVED"
#define MESSAGE_STATUS_PROCESSING "PROCESSING"
#define MESSAGE_STATUS_NOT_FOUND "NOT_FOUND"
#define MESSAGE_STATUS_SHIPPED "SHIPPED"

/**
 * Base class for messages
 */
class Message {
public:
    virtual MessageType type() const = 0;
};

/**
 * Base class for response messages
 */
class ResponseMessage : public Message {
public:
	const int clientID;
    const std::string status;
    const std::string info;

    explicit ResponseMessage(const int& clientID, const std::string &status,
                    const std::string &info = "") :
            clientID(clientID), status(status), info(info) {}

};

/**
 * Add a quote to central computer
 */
class AddMessage : public Message {
public:
    const Quote quote;
	const int clientID;

    explicit AddMessage(const Quote &quote) : quote(quote), clientID(-1) {}

	explicit AddMessage(const Quote &quote, int& clientID) : 
		quote(quote), clientID(clientID) {}

    MessageType type() const override {
        return MessageType::ADD;
    }
};

/**
 * Response to adding a quote to central computer
 */
class AddResponseMessage : public ResponseMessage {
public:
    const AddMessage add;
	const std::string orderID;

    AddResponseMessage(const AddMessage &add, const std::string orderID, int clientID, 
						const std::string status, std::string info = "") :
            ResponseMessage(clientID, status, info), add(add), orderID(orderID) {}

    MessageType type() const override {
        return MessageType::ADD_RESPONSE;
    }
};

/**
 * Remove an order from the central computer
 */
class RemoveMessage : public Message {
public:
	const std::string orderID;
	const int clientID;

	RemoveMessage(const std::string &orderID) : orderID(orderID), clientID(-1) {}

	RemoveMessage(const std::string &orderID, int& clientID) : orderID(orderID),
		clientID(clientID) {}

	MessageType type() const override {
		return MessageType::REMOVE;
	}
};

/**
 * Response to removing an order from the central computer
 */
class RemoveResponseMessage : public ResponseMessage {
public:
    const RemoveMessage remove;

    RemoveResponseMessage(const RemoveMessage &remove, const int clientID, 
							const std::string status, std::string info = "") :
            ResponseMessage(clientID, status, info), remove(remove) {}

    MessageType type() const override {
        return MessageType::REMOVE_RESPONSE;
    }
};

/**
 * Remove an order from the central computer
 */
class RemoveItemMessage : public Message {
public:
	const std::string orderID;
	const std::string itemName;
	const int clientID;

	RemoveItemMessage(const std::string &orderID, const std::string &itemName)
		: orderID(orderID), itemName(itemName), clientID(-1) {}

	RemoveItemMessage(const std::string &orderID, const std::string &itemName, int& clientID)
		: orderID(orderID), itemName(itemName), clientID(clientID) {}

	MessageType type() const override {
		return MessageType::REMOVE_ITEM;
	}
};

/**
 * Response to removing an order from the central computer
 */
class RemoveItemResponseMessage : public ResponseMessage {
public:
    const RemoveItemMessage removeItem;

    RemoveItemResponseMessage(const RemoveItemMessage &removeItem, const int clientID,  
								const std::string status, std::string info = "") :
            ResponseMessage(clientID, status, info), removeItem(removeItem) {}

    MessageType type() const override {
        return MessageType::REMOVE_ITEM_RESPONSE;
    }
};

/**
 * Search the order in the central computer using regular expressions
 */
class SearchMessage : public Message {
public:
	const std::string orderID;
	const int clientID;

	SearchMessage(const std::string &orderID) :
		orderID(orderID), clientID(-1) {}

	SearchMessage(const std::string &orderID, int& clientID) :
		orderID(orderID), clientID(clientID) {}

	MessageType type() const override {
		return MessageType::SEARCH;
	}
};

/**
 * Response to a central computer order search
 */
class SearchResponseMessage : public ResponseMessage {
public:
	const SearchMessage search;
	//const std::string results;

	SearchResponseMessage(const SearchMessage &search, const int clientID,
		const std::string &status, const std::string &info = "") :
		ResponseMessage(clientID, status, info), search(search) {}

	MessageType type() const override {
		return MessageType::SEARCH_RESPONSE;
	}
};

/**
 * Search the central computer using regular expressions
 */
class SearchItemMessage : public Message {
public:
	const std::string itemName;
	const int clientID;

	SearchItemMessage(const std::string &itemName) :
		itemName(itemName), clientID(-1) {}

	SearchItemMessage(const std::string &itemName, int& clientID) :
		itemName(itemName), clientID(clientID) {}

	MessageType type() const override {
		return MessageType::SEARCH_ITEM;
	}
};

/**
 * Response to a central computer order search
 */
class SearchItemResponseMessage : public ResponseMessage {
public:
	const SearchItemMessage searchItem;
	//const std::string results; //--> use .info

	SearchItemResponseMessage(const SearchItemMessage &searchItem, const int clientID,
		const std::string &status, const std::string &info = "") :
		ResponseMessage(clientID, status, info), searchItem(searchItem) {}

	MessageType type() const override {
		return MessageType::SEARCH_ITEM_RESPONSE;
	}
};

/**
 * Goodbye message
 */
class GoodbyeMessage : public Message {
public:
	const int clientID;

	GoodbyeMessage() : clientID(-1) {}

	GoodbyeMessage(const int clientID) : clientID(clientID) {}

	MessageType type() const override {
		return MessageType::GOODBYE;
	}
};

#endif //PROJECT_MESSAGES_H
