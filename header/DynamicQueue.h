//
//  DynamicQueue.h
//

#ifndef DYNAMICQUEUE_H
#define DYNAMICQUEUE_H

#include "Order.h"
#include "Item.h"
#include "Quote.h"
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>

template <class T>
class DynamicQueue{
	protected:
		std::deque<T> buff_;
		std::mutex mutex_;
		std::condition_variable cv_;
		
	public:
		/**
		* Creates the dynamic queue
		*/
		DynamicQueue():buff_(), mutex_(), cv_() {}
			
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

class ItemQueue: public DynamicQueue<Item>{
	public:
		/**
		* Creates the dynamic item queue
		*/
		ItemQueue() {}
};

class QuoteQueue: public DynamicQueue<Quote>{
	public:
		/**
		* Creates the dynamic quote queue
		*/
		QuoteQueue(){}
};

//class TruckQueue: public DynamicQueue<TruckInfo>{
//	public:
//		/**
//		* Creates the dynamic truck queue
//		*/
//		TruckQueue(){}
//};

class MessageQueue: public DynamicQueue<std::string>{
	public:
		/**
		* Creates the dynamic message queue
		*/
		MessageQueue() {}
};

class OrderQueue: public DynamicQueue<Order>{
	private:
		int processIndex;
	public:
		/**
		* Creates the dynamic order queue
		*/
		OrderQueue(): DynamicQueue<Order>() {
			buff_ = std::deque<Order>(); // initialize buff_
			processIndex = 0;
		}
		
		/**/
		OrderQueue(std::vector<Order>& orders): DynamicQueue<Order>(){
			buff_ = std::deque<Order>(); // initialize buff_

			processIndex = 0;

			// push orders into buff_
			for (Order& od : orders) {
				buff_.push_back(od);
			}
		}

		bool addOrder(Order& od) {
			try {
					{
						std::lock_guard<decltype(mutex_)> lock(mutex_);
						buff_.push_back(od);
					}
				return true;
			}
			catch (std::exception &e) {
				std::cerr << e.what();
				return false;
			}
		}

		/**
		 * get reference to order at processIndex
		 * get is NOT destructive
		 * processIndex++ after a successful get
		 * @param outorder reference to outorder
		 * @return true if success, false if failed
		 */
		bool getOrder(Order& outorder) {
			if (buff_.size() > 0) {
				outorder = buff_.at(processIndex);
				{
					std::lock_guard<decltype(mutex_)> lock(mutex_);
					processIndex++;
				}
				return true;
			}
			else {
				return false;
			}
		}

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
		bool searchOrderID(std::string orderID, Order& outorder){
			for (Order& od : buff_) {
				if (orderID == od.returnOrderID()) {
					outorder = od;
					return true;
				}
			}
			return false;
		}
};

#endif // DYNAMICQUEUE_H
