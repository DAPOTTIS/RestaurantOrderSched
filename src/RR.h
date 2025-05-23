#pragma once // Use pragma once

#include <vector> // RR uses vector
#include <mutex>
#include <condition_variable>
#include <optional> // For std::optional
#include "Order.h"
#include "Menu.h"
#include "Timer.h"

class RR {
public:
    RR();
    RR(int quantum); // Constructor to set time quantum
    void addOrder(Order& order);
    void addOrder(const Menu& menu); // Interactive addOrder
    void start();
    void stop();
    size_t getOrderCount(); // Original name

    // For "upcoming" orders queue
    static std::vector<Order> getQueue(); // Keeps original name

    // For the currently processing order for GUI
    static std::optional<Order> getCurrentlyProcessingOrder();

    // For completed orders
    static std::vector<Order> getCompletedOrders();

    Timer timer; // Timer instance for wait time tracking

private:
    void processOrders();

    // Static members for the queue of upcoming/partially processed orders
    static std::vector<Order> orderList; // RR uses vector
    static std::mutex listMutex; // Protects orderList
    static std::condition_variable cv;
    static bool isProcessing;
    static int timeQuantum;       
    static bool threadStarted; // To ensure only one thread is started

    // Static members for currently processing order
    static std::optional<Order> s_currentlyProcessingOrder;
    static std::mutex s_currentOrderMutex; // Protects s_currentlyProcessingOrder

    // Static members for completed orders
    static std::vector<Order> s_completedOrders;
    static std::mutex s_completedOrdersMutex; // Protects s_completedOrders

    static SchedulerType schedulerType; // Scheduler type for the order

};
