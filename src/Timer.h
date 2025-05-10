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

enum SchedulerType {
    FCFS_,
    PQ_,
    RR_
};

class Timer {
public:

    static double totalWaitTime;
    static std::vector<double> totalWaitTimes;

    void startwaitTimer(Order& order, SchedulerType scheduler){
            // std::lock_guard<std::mutex> lock(mutex); // Reverted: lock commented out
            int orderID = order.getOrderId();
            resizeIfNeeded(orderID);
            startTime[orderID] = std::chrono::steady_clock::now();
        
            std::cout << "[Timer] Started wait timer for order ID: " << orderID
                      << " at time: " << std::chrono::duration_cast<std::chrono::microseconds>(startTime[orderID].time_since_epoch()).count() << " us" << std::endl;
    }

    void stopwaitTimer(Order& order, SchedulerType scheduler){
            // std::lock_guard<std::mutex> lock(mutex); // Reverted: lock commented out
            int orderID = order.getOrderId();
            resizeIfNeeded(orderID);
            endTime[orderID] = std::chrono::steady_clock::now();
        
            std::cout << "[Timer] Stopped wait timer for order ID: " << orderID
                      << " at time: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime[orderID].time_since_epoch()).count() << " us" << std::endl;
    }
    void calcualteWaitTimer(Order& order, SchedulerType scheduler){
        // Reverted: std::lock_guard<std::mutex> lock(mutex); removed
        int orderID = order.getOrderId();
        resizeIfNeeded(orderID);
        // Reverted: Conditional check (if (orderID < startTime.size() && ...)) removed
        std::chrono::duration<double> elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(endTime[orderID] - startTime[orderID]);
        order.waitingTime += elapsed_seconds.count();
        totalWaitTime += elapsed_seconds.count();
        // Ensure totalWaitTimes is properly initialized and sized in its definition (e.g., Timer.cpp)
        // to prevent out-of-bounds access here.
        if (scheduler >= 0 && static_cast<size_t>(scheduler) < totalWaitTimes.size()) {
             totalWaitTimes[scheduler] += elapsed_seconds.count();
        } else {
            std::cerr << "[Timer] Error: SchedulerType out of bounds for totalWaitTimes access in calcualteWaitTimer." << std::endl;
        }
        std::cout << "Total wait time: " << totalWaitTime << std::endl;
        std::cout << "Order ID: " << order.getOrderId() << "Current Wait time: " << order.waitingTime << std::endl;
        std::cout << "[Timer] Calculated wait time for order ID: " << orderID
                  << ": " << elapsed_seconds.count() << " seconds" << std::endl;
        // Reverted: else block for error handling removed
    }

private:
    // std::unordered_map<int, OrderTimes> orders_;
    std::mutex mutex; // Mutex for protecting shared data (currently unused due to commented locks)
    // static std::condition_variable cv; // This seems unused, consider removing if not needed
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