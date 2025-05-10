#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include "Order.h"
#include <iostream>
#include <vector>

class Timer {
public:

    static double totalWaitTime;

    void startwaitTimer(Order& order){
            
            // std::lock_guard<std::mutex> lock(mutex);
            int orderID = order.getOrderId();
            resizeIfNeeded(orderID);
            startTime[orderID] = std::chrono::steady_clock::now();
        
            std::cout << "[Timer] Started wait timer for order ID: " << orderID
                      << " at time: " << std::chrono::duration_cast<std::chrono::microseconds>(startTime[orderID].time_since_epoch()).count() << " us" << std::endl;
    }

    void stopwaitTimer(Order& order){
        
            // std::lock_guard<std::mutex> lock(mutex);
            int orderID = order.getOrderId();
            resizeIfNeeded(orderID);
            endTime[orderID] = std::chrono::steady_clock::now();
        
            std::cout << "[Timer] Stopped wait timer for order ID: " << orderID
                      << " at time: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime[orderID].time_since_epoch()).count() << " us" << std::endl;
    }
    void calcualteWaitTimer(Order& order){
        int orderID = order.getOrderId();
        resizeIfNeeded(orderID);
        std::chrono::duration<double> elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(endTime[orderID] - startTime[orderID]);
        order.waitingTime += elapsed_seconds.count();
        totalWaitTime += elapsed_seconds.count();
        std::cout << "Total wait time: " << totalWaitTime << std::endl;
        std::cout << "Order ID: " << order.getOrderId() << "Current Wait time: " << order.waitingTime << std::endl;
        std::cout << "[Timer] Calculated wait time for order ID: " << orderID
                  << ": " << elapsed_seconds.count() << " seconds" << std::endl;
    }

private:
    // std::unordered_map<int, OrderTimes> orders_;
    std::mutex mutex;
    static std::condition_variable cv;
    std::vector<std::chrono::steady_clock::time_point> startTime;
    std::vector<std::chrono::steady_clock::time_point> endTime;

    void resizeIfNeeded(int orderID) {
        if (orderID >= startTime.size()) {
            startTime.resize(orderID + 1);
        }
        if (orderID >= endTime.size()) {
            endTime.resize(orderID + 1);
        }
    }

};

#endif // TIMER_H