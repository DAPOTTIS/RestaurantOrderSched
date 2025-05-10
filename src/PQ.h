#pragma once

#include <deque>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <optional> // Include for std::optional
#include "Order.h" // Assuming Order.h is correctly included by PQ.cpp
#include "Menu.h"  // Assuming Menu.h is correctly included by PQ.cpp


class PriorityScheduler {
public:
    PriorityScheduler();
    void addOrder(const Order& order);
    void addOrder(const Menu& menu);
    void start();
    void stop();
    size_t getOrderCount() const;

    // For "upcoming" orders
    std::deque<Order> getHighPriorityQueueCopy() const;
    std::deque<Order> getLowPriorityQueueCopy() const;

    // For the currently processing order for GUI
    static std::optional<Order> getCurrentlyProcessingOrder();

    // For completed orders
    static std::vector<Order> getCompletedOrders();


private:
    void processOrders();

    static std::deque<Order> highPriorityQueue;
    static std::deque<Order> lowPriorityQueue;
    static std::mutex queueMutex; // Protects highPriorityQueue and lowPriorityQueue
    static std::condition_variable cv;
    static bool isProcessing; 
    static bool threadStarted; 

    // Static members for currently processing order
    static std::optional<Order> s_currentlyProcessingOrder;
    static std::mutex s_currentOrderMutex; // Protects s_currentlyProcessingOrder

    // Static members for completed orders
    static std::vector<Order> s_completedOrders;
    static std::mutex s_completedOrdersMutex; // Protects s_completedOrders

    static const int PRIORITY_THRESHOLD = 6; 
};
