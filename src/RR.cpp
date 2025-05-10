#include "RR.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm> // For std::min

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

void RR::addOrder(const Order& order) {
    {
        std::lock_guard<std::mutex> lock(listMutex); // Protects orderList
        orderList.push_back(order);
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
        const auto& item = menu.getItemById(x);
        this->addOrder(Order(item)); 
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
                orderList.push_back(localCurrentOrder); // Re-queue the modified local copy
                reQueued = true;
            } else {
            //    cout << "[RR] Completed Order ID: " << localCurrentOrder.getOrderId() << "\n";
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
