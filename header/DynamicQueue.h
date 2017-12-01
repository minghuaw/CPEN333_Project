/**
 * @file 
 * This document defines the thread-safe dynamic queue class
 * 
 */

#ifndef DYNAMICQUEUE_H
#define DYNAMICQUEUE_H

#include "Order.h"
#include "Item.h"
#include "Quote.h"
#include "TruckInfo.h"


template <class T>

class DynamicQueue{
	private:
		std::deque<T> buff_;
		std::mutex mutex_;
		std::condition_variable cv_;
		
	public:
		/**
		* Creates the dynamic queue
		*/
		DynamicOrderQueue() :
			buff_(), mutex_(), cv_(){}
			
		/**
		* Adds an template obj to the queue
		* @param template obj to add
		*/
		void add(T& obj) {
			{
				std::lock_guard<std::mutex> lock(mutex_);
				buff_.push_back(obj);
			}
			cv_.notify_all();
		}

		/**
		* Retrieve the next order template obj in the queue
		* @return next available template obj
		*/
		T get(){
			T out;
			{
			std::unique_lock<std::mutex> lock(mutex_);
				while (buff_.size()==0) {
					cv_.wait(lock);
				}
				// get first item in queue
				out = buff_.front();
				buff_.pop_front();
			}

			return out;
		}

};

class ItemQueue: public DynamicQueue{
	public:
		/**
		* Creates the dynamic item queue
		*/
		ItemQueue(): SuperClass() {}
};

class QuoteQueue: public DynamicQueue{
	public:
		/**
		* Creates the dynamic quote queue
		*/
		QuoteQueue(): SuperClass() {}
};

class TruckQueue: public DynamicQueue{
	public:
		/**
		* Creates the dynamic truck queue
		*/
		TruckQueue(): SuperClass() {}
};

class MessageQueue: public DynamicQueue{
	public:
		/**
		* Creates the dynamic message queue
		*/
		MessageQueue(): SuperClass() {}
}

class OrderQueue: public DynamicQueue{
	private:
		int processIndex;
	public:
		/**
		* Creates the dynamic order queue
		*/
		OrderQueue(): SuperClass() {}
		
		/**
		* Try to get an order that weighs less than or equal to the specified weight
		* @param weight		maximum weight of order
		* @param outorder	the order found that meets the requirement
		* @return			true if an order less than or equal to specified weight can be found, false otherwise
		*/
		bool tryGet(double weight, Order& outorder){
		}
		
		/**
		* Swap two orders in queue at specified indexes
		* @param ind1		index of first order
		* @param ind2		index of second order
		*/
		bool swapOrder(int ind1, int ind2){
		}
		
		/**
		* search order with ID
		* @param orderID	ID of order
		* @param outorder	the ordre found to match the ID
		* @return True if an order if found, false otherwise
		*/
		bool searchOrderID(orderID : std::string, Order& outorder){
		}
};

#endif // DYNAMICQUEUE_H
