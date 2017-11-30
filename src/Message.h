/**
 * @file
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
#include <string>

/**
 * Types of messages that can be sent between client/server
 */
enum MessageType {
  ADD,
  ADD_RESPONSE,
  REMOVE,
  REMOVE_RESPONSE,
  SEARCH,
  SEARCH_RESPONSE,
  GOODBYE,
  UNKNOWN
};

// status messages for response objects
#define MESSAGE_STATUS_OK "OK"
#define MESSAGE_STATUS_ERROR "ERROR"

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
  const std::string status;
  const std::string info;
  ResponseMessage(const std::string& status,
                  const std::string& info="") :
      status(status), info(info){}

};

/**
 * Add a order to central computer
 */
class AddMessage : public Message {
 public:
  const Order order;

  AddMessage(const Order& order)  : order(order) {}

  MessageType type() const {
    return MessageType::ADD;
  }
};

/**
 * Response to adding a order to centreal computer
 */
class AddResponseMessage : public ResponseMessage {
 public:
  const AddMessage  add;

  AddResponseMessage(const AddMessage& add, std::string status, std::string info = "") :
      ResponseMessage(status, info), add(add) {}

  MessageType type() const {
    return MessageType::ADD_RESPONSE;
  }
};

/**
 * Remove an order from the central computer
 */
class RemoveMessage : public Message {
 public:
  const Order order;

  RemoveMessage(const Order& order) : order(order) {}

  MessageType type() const {
    return MessageType::REMOVE;
  }
};

/**
 * Response to removing an order from the central computer
 */
class RemoveResponseMessage : public ResponseMessage {
 public:
  const RemoveMessage remove;

  RemoveResponseMessage(const RemoveMessage& remove, std::string status, std::string info = "") :
      ResponseMessage(status, info), remove(remove) {}

  MessageType type() const {
    return MessageType::REMOVE_RESPONSE;
  }
};

/**
 * Search the central computer using regular expressions
 */
class SearchMessage : public Message {
 public:
  const std::string item_name;

  SearchMessage(const std::string& item_name) :
      item_name(item_name) {}

  MessageType type() const {
    return MessageType::SEARCH;
  }
};

/**
 * Response to a central computer order search
 */
class SearchResponseMessage : public ResponseMessage {
 public:
  const SearchMessage search;
  const std::vector<Order> results;

  SearchResponseMessage(const SearchMessage& search, const std::vector<Order>& results,
    const std::string& status, const std::string& info = "" ) :
      ResponseMessage(status, info), search(search), results(results) {}

  MessageType type() const {
    return MessageType::SEARCH_RESPONSE;
  }
};

/**
 * Goodbye message
 */
class GoodbyeMessage : public Message {
 public:
  MessageType type() const {
    return MessageType::GOODBYE;
  }
};

#endif //PROJECT_MESSAGES_H
