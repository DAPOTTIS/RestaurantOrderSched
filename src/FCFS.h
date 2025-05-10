#pragma once // Use pragma once for include guards

#include <deque>
#include <vector> // For std::vector
#include <mutex>
#include <condition_variable>
#include <optional> // For std::optional
#include "Order.h" // Assuming Order is defined here
#include "Menu.h"  // Assuming Menu is defined here

class FCFS {
public:
    FCFS();
    void addOrder(const Order& order);
    void addOrder(const Menu& menu); // Interactive addOrder
    void start();
    void stop();
    size_t getQueueSize(); // Renamed for clarity, matches existing use

    // For "upcoming" orders queue
    static std::deque<Order> getQueue(); // Keeps original name if Application.cpp uses this

    // For the currently processing order for GUI
    static std::optional<Order> getCurrentlyProcessingOrder();

    // For completed orders
    static std::vector<Order> getCompletedOrders();

private:
    void processOrders();

    // Static members for the queue of upcoming orders
    static std::deque<Order> orderQueue;
    static std::mutex queueMutex; // Protects orderQueue
    static std::condition_variable cv;
    static bool isProcessing;
    static bool threadStarted; // To ensure only one thread is started

    // Static members for currently processing order
    static std::optional<Order> s_currentlyProcessingOrder;
    static std::mutex s_currentOrderMutex; // Protects s_currentlyProcessingOrder

    // Static members for completed orders
    static std::vector<Order> s_completedOrders;
    static std::mutex s_completedOrdersMutex; // Protects s_completedOrders
};
