#include "FCFS.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>  // For std::vector
// #include <mutex> // Already included via FCFS.h if it includes <mutex>
// #include "Menu.h" // Already included via FCFS.h

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

// Static member definitions for the upcoming orders queue
bool FCFS::isProcessing = false;
std::deque<Order> FCFS::orderQueue;
std::mutex FCFS::queueMutex;
std::condition_variable FCFS::cv;
bool FCFS::threadStarted = false; // Initialize threadStarted

// Static member definitions for currently processing order
std::optional<Order> FCFS::s_currentlyProcessingOrder;
std::mutex FCFS::s_currentOrderMutex;

// Static member definitions for completed orders
std::vector<Order> FCFS::s_completedOrders;
std::mutex FCFS::s_completedOrdersMutex;

SchedulerType FCFS::schedulerType = SchedulerType::FCFS_; // Initialize scheduler type

FCFS::FCFS(){}

// New public static method
std::optional<Order> FCFS::getCurrentlyProcessingOrder() {
    std::lock_guard<std::mutex> lock(s_currentOrderMutex);
    return s_currentlyProcessingOrder;
}

void FCFS::addOrder(Order& order) {
    {
        std::lock_guard<std::mutex> lock(queueMutex); // Protects orderQueue
        orderQueue.push_back(order);
        timer.startwaitTimer(order, schedulerType); // Start wait timer for the new order
    }
    cv.notify_one(); 
    //cout << "New order added! Current in queue is " << getQueueSize() << endl;
}

void FCFS::addOrder(const Menu& menu) {
    while(true){
        int x = -1;
        sleep_for(std::chrono::milliseconds(2)); 
        menu.display();
        cout << endl << "Enter the ID of the item you want to order: (0 to exit)" << endl;;
        cin >> x;
        if (x == 0) {
            this->stop();
            // exit(0); // Avoid exit(0) in library code
            return; 
        }
        if (x < 1 || x > static_cast<int>(menu.getItemsCount())) {
            cout << endl << "Invalid selection. Please re-enter a valid value." << endl;
            continue;
        }
        MenuItem item = menu.getItemById(x);
        Order order(item);
        this->addOrder(order);
    }
}

void FCFS::processOrders() {
    isProcessing = true;
    while (isProcessing || !orderQueue.empty()) { // Ensure loop continues if orders exist even if stop is called
        Order localCurrentOrder;
        bool orderFound = false;
        {
            std::unique_lock<std::mutex> lock(queueMutex); // Protects orderQueue
            cv.wait(lock, [this](){ return !orderQueue.empty() || !isProcessing; });
            
            if (!isProcessing && orderQueue.empty()) {
                break;
            }
            if (orderQueue.empty()) { // Should be caught by cv.wait unless stop was called
                continue;
            }

            localCurrentOrder = orderQueue.front();
            orderQueue.pop_front(); // POPPED HERE
            orderFound = true;
        }

        if (orderFound) {
            { // Set currently processing order
                std::lock_guard<std::mutex> currentLock(s_currentOrderMutex);
                s_currentlyProcessingOrder = localCurrentOrder;
            }
            
            // sleep_for(std::chrono::milliseconds(1)); // Avoid print conflict if any logs were active
            // cout << "Processing Order ID: " << localCurrentOrder.getOrderId() 
            //      << " - " << localCurrentOrder.getItemName() 
            //      << " - Burst Time: " << localCurrentOrder.getPrepTime() << " minutes" << endl;

            // cout << "[FCFS] Stopping wait timer for order ID: " << localCurrentOrder.getOrderId() << endl;
            timer.stopwaitTimer(localCurrentOrder, schedulerType); // Stop wait timer for the order
            sleep_for(seconds(localCurrentOrder.getPrepTime())); // Simulate processing
            // cout << "[FCFS] Calculating wait timer for order ID: " << localCurrentOrder.getOrderId() << endl;
            timer.calcualteWaitTimer(localCurrentOrder, schedulerType); // Calculate wait time for the order
            
            // cout << "Completed Order ID: " << localCurrentOrder.getOrderId() << endl;
            // cout << "Remaining in queue is " << getQueueSize() << endl;

            { // Add to completed orders
                std::lock_guard<std::mutex> lock(s_completedOrdersMutex);
                s_completedOrders.push_back(localCurrentOrder);
            }

            { // Clear currently processing order
                std::lock_guard<std::mutex> currentLock(s_currentOrderMutex);
                s_currentlyProcessingOrder.reset();
            }
        }
    }
}

void FCFS::start() {
    // Protect threadStarted with queueMutex or a dedicated mutex if preferred
    std::lock_guard<std::mutex> lock(queueMutex); 
    if (threadStarted) return; 
    threadStarted = true;
    
    std::thread processingThread(&FCFS::processOrders, this);
    processingThread.detach(); 
}

void FCFS::stop() {
    {
        std::lock_guard<std::mutex> lock(queueMutex); // Protects orderQueue
        isProcessing = false;
    }
    cv.notify_all(); 
}

size_t FCFS::getQueueSize() {
    std::lock_guard<std::mutex> lock(queueMutex); // Protects orderQueue
    return orderQueue.size();
}

std::deque<Order> FCFS::getQueue() { // Gets the "upcoming" orders queue
    std::lock_guard<std::mutex> lock(queueMutex); // Protects orderQueue
    return orderQueue;
}

// New public static method to get completed orders
std::vector<Order> FCFS::getCompletedOrders() {
    std::lock_guard<std::mutex> lock(s_completedOrdersMutex);
    return s_completedOrders;
}