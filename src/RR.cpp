#include "RR.h"
#include "Timer.h" // Ensure Timer.h is included for SchedulerType::RR_
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm> // For std::min
#include <vector>    // For std::vector

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

// Static member definitions for the upcoming/partially processed orders queue
std::vector<Order> RR::orderList;
std::mutex RR::listMutex;
std::condition_variable RR::cv;
bool RR::isProcessing = false;
int RR::timeQuantum = 8; // Default time quantum
bool RR::threadStarted = false; // Initialize threadStarted

// Static member definitions for currently processing order
std::optional<Order> RR::s_currentlyProcessingOrder;
std::mutex RR::s_currentOrderMutex;

// Static member definitions for completed orders
std::vector<Order> RR::s_completedOrders;
std::mutex RR::s_completedOrdersMutex;

RR::RR() {}

RR::RR(int quantum) {
    // Consider if timeQuantum should be instance member or static.
    // If static, this constructor sets it for all instances.
    // If your design implies one RR scheduler, static is fine.
    timeQuantum = quantum;
}

// New public static method
std::optional<Order> RR::getCurrentlyProcessingOrder() {
    std::lock_guard<std::mutex> lock(s_currentOrderMutex);
    return s_currentlyProcessingOrder;
}

void RR::addOrder(Order& order) {
    {
        std::lock_guard<std::mutex> lock(listMutex); // Protects orderList
        orderList.push_back(order);
        // Start wait timer when order is added to the queue
        this->timer.startwaitTimer(order, RR_);
        this->timer.startTurnaroundTimer(order, RR_); // Start turnaround timer
    }
    cv.notify_one();
    //cout << "Total orders in Queue: " << getOrderCount() << endl;
}

void RR::addOrder(const Menu& menu) {
    while (true) {
        int x = -1;
        sleep_for(milliseconds(2));
        menu.display();
        cout << "\nEnter item ID to order (0 to exit): ";
        cin >> x;

        if (x == 0) {
            this->stop(); 
            // exit(0); // Avoid exit(0)
            return;
        }

        if (x < 1 || x > static_cast<int>(menu.getItemsCount())) {
            cout << "Invalid item ID. Try again.\n";
            continue;
        }
        MenuItem item = menu.getItemById(x);
        Order order(item);
        this->addOrder(order);
    }
}

void RR::processOrders() {
    isProcessing = true;
    while (isProcessing || !orderList.empty()) { // Ensure loop continues if orders exist
        Order localCurrentOrder;
        bool orderFound = false;

        {
            std::unique_lock<std::mutex> lock(listMutex); // Protects orderList
            cv.wait(lock, [this] { return !orderList.empty() || !isProcessing; });
            
            if (!isProcessing && orderList.empty()) {
                break;
            }
            if (orderList.empty()) {
                continue;
            }
            
            localCurrentOrder = orderList.front();
            orderList.erase(orderList.begin()); // POPPED HERE (vector uses erase)
            orderFound = true;

            // Stop wait timer and calculate wait time as order is picked for processing
            this->timer.stopwaitTimer(localCurrentOrder, RR_);
            this->timer.calcualteWaitTimer(localCurrentOrder, RR_);
        }
        
        if (orderFound) {
            { // Set currently processing order
                std::lock_guard<std::mutex> currentLock(s_currentOrderMutex);
                s_currentlyProcessingOrder = localCurrentOrder;
            }
            
            int runTime = std::min(timeQuantum, localCurrentOrder.getRemainingTime());
            // cout << "\n[RR] Processing Order ID: " << localCurrentOrder.getOrderId()\n            //    << " - " << localCurrentOrder.getItemName()\n            //     << " for " << runTime << " minutes (Remaining before: "\n            //     << localCurrentOrder.getRemainingTime() << ")\n";
            
            sleep_for(seconds(runTime)); 
            
            localCurrentOrder.reduceRemainingTime(runTime); // Modify the local copy

            bool reQueued = false;
            if (localCurrentOrder.getRemainingTime() > 0) {
                std::lock_guard<std::mutex> lock(listMutex); // Protects orderList
                // Start wait timer again if order is re-queued
                this->timer.startwaitTimer(localCurrentOrder, RR_);
                orderList.push_back(localCurrentOrder); // Re-queue the modified local copy
                reQueued = true;
            } else {
            //    cout << "[RR] Completed Order ID: " << localCurrentOrder.getOrderId() << "\n";
                // Order completed
                this->timer.stopTurnaroundTimer(localCurrentOrder, RR_);
                this->timer.calculateTurnaroundTime(localCurrentOrder, RR_);
                // Note: calcualteWaitTimer was already called when order was picked.
                // If further calculation specific to completion is needed, it would go here.
                // However, the current Timer design calculates wait time upon dequeue.
                { // Add to completed orders
                    std::lock_guard<std::mutex> lock(s_completedOrdersMutex);
                    s_completedOrders.push_back(localCurrentOrder);
                }
            }

            { // Clear currently processing order
                std::lock_guard<std::mutex> currentLock(s_currentOrderMutex);
                s_currentlyProcessingOrder.reset();
            }
        }
    }
}

void RR::start() {
    std::lock_guard<std::mutex> lock(listMutex); // Protect threadStarted
    if (threadStarted) return;
    threadStarted = true;

    thread t(&RR::processOrders, this);
    t.detach();
}

void RR::stop() {
    {
        std::lock_guard<std::mutex> lock(listMutex); // Protects orderList
        isProcessing = false;
    }
    cv.notify_all();
}

size_t RR::getOrderCount() {
    std::lock_guard<std::mutex> lock(listMutex); // Protects orderList
    return orderList.size();
}

std::vector<Order> RR::getQueue() { // Gets the "upcoming" / "partially processed" orders queue
    std::lock_guard<std::mutex> lock(listMutex); // Protects orderList
    return orderList;
}

// New public static method to get completed orders
std::vector<Order> RR::getCompletedOrders() {
    std::lock_guard<std::mutex> lock(s_completedOrdersMutex);
    return s_completedOrders;
}
