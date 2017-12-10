//
// Created by minghua on 09/12/17.
//

#ifndef AMAZOOM_WAREHOUSECOMPUTERAPI_H
#define AMAZOOM_WAREHOUSECOMPUTERAPI_H

#define JSON_ID 55

#include <cpen333/process/mutex.h>
#include <cpen333/process/socket.h>
#include <mutex>
#include <string>
#include <iostream>
#include "Amazoom.h"
#include "Message.h"
#include <json.hpp>
#include <deque>
#include "jsonConverter.h"

using JSON = nlohmann::json;

class WarehouseComputerAPI{
private:
    cpen333::process::socket socket_;

    bool readString(std::string& str, size_t size){
        char cbuff[256];
        size_t buffSize = sizeof(cbuff);

        bool success = false;

        int num2read = std::min(buffSize, size);
        while (socket_.read_all(cbuff, num2read)){
            for(int i=0; i<num2read; i++){
                str += cbuff[i];
            }

            if (!success)
                success = true;

            if (size>buffSize)
                size -= buffSize;
            else
                break;

            num2read = std::min(buffSize, size);
        }

        return success;
    }
public:
	/**
	 * defautl constructor
	 */
	WarehouseComputerAPI() {}

    /**
     * Constructor for WarehouseComputerAPI
     * @param client
     */
    WarehouseComputerAPI(cpen333::process::socket&& client) : socket_(std::move(client)){}

    /**
     * send JSON string message to warehouseComputer
     */
    void sendMessage(){}

	/**
	* send JSON string message to warehouseComputer
	*/
	bool sendMessage(const Message& msg) {
		JSON jmsg = JsonConverter::toJSON(msg);
		return sendJSON(jmsg);
	}

    /**
     * send JSON string
     * @param jsonStr
     * @return
     */
    bool sendJSON(std::string jsonStr){
        // encode json string size
        char jsonID = JSON_ID;
        char buff[4];
        size_t size = jsonStr.size()+1;

        for (int i=4; i-->0;){
            // cut off byte and shift size over by 8 bits
            buff[i] = (char)(size & 0xff);
            size = size >> 8;
        }

        bool success = socket_.write(&jsonID, 1); // write start byte
        if (success){
            success &= socket_.write(buff, 4); // content size
            success &= socket_.write(jsonStr); // content
        }
        return success;
    }

    /**
     *
     * @param j json object
     */
    bool sendJSON(const JSON& j){
        std::string jsonStr = j.dump();

        return sendJSON(jsonStr);
    }


	//TODO: 

    /**
     *
     * @return parsed message, nullptr if an error occurred
     */
    std::unique_ptr<Message> recvMessage(){}

    /**
     *
     * @return unique pointer to JSON string, nullptr if error occurs
     */
    std::unique_ptr<std::string> recvJSON(){
        char id;
        char buff[4];
        size_t size = 0;
        if (!socket_.read_all(&id, 1) || id != JSON_ID){
            std::cout << "JSON ID does not match" << std::endl;
            return nullptr;
        }

        // if if is a json string
        if (!socket_.read_all(buff, 4)) {
            std::cout << "Cannot read JSON size" << std::endl;
            return nullptr;
        }

        for (int i = 0; i < 4; i++) {
            char buff_char = buff[4 - 1 - i];// << 8 * i;
            int temp = ((unsigned int)buff_char & 0xFF);
            temp = temp << 8 * i;
            size += temp;
        }

        // read entire JSON string
        std::string str;
        if (!readString(str, size)) {
            return nullptr;
        }

		std::cout << str << std::endl;

        return std::unique_ptr<std::string>(new std::string(str));
    }

};

#endif //AMAZOOM_WAREHOUSECOMPUTERAPI_H
