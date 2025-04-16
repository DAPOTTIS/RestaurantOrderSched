#include "FCFS.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include "Menu.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

// int FCFS::id = 0; // Initialize static variable
bool FCFS::isProcessing = false;
std::deque<Order> FCFS::orderQueue;
std::mutex FCFS::queueMutex;
std::condition_variable FCFS::cv;
FCFS::FCFS(){}

void FCFS::addOrder(const Order& order) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        orderQueue.push_back(order);
    }
    cv.notify_one(); // notify waiting thread that a new order is available
    cout << "New order added! Current in queue is " << getQueueSize() << endl;

}

void FCFS::addOrder(const Menu& menu) {
    while(true){
        int x = -1;
        sleep_for(std::chrono::milliseconds(2)); // Avoid print conflict
        menu.display();
        cout << endl << "Enter the ID of the item you want to order: (0 to exit)" << endl;;
        cin >> x;
        if (x == 0) {
            this->stop();
            exit(0);
        }
        if (x < 1 || x > static_cast<int>(menu.getItemsCount())) {
            cout << endl << "Invalid selection. Please re-enter a valid value." << endl;
            continue;
        }
        const auto& item = menu.getItemById(x);
        this->addOrder(Order(item));
    }
}

void FCFS::processOrders() {
    isProcessing = true;
    while (isProcessing) {
        Order currentOrder;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this](){ return !orderQueue.empty() || !isProcessing; });
            if (!isProcessing && orderQueue.empty())
                break;
            currentOrder = orderQueue.front();
        }
        sleep_for(std::chrono::milliseconds(1)); // Avoid print conflict
        cout << "Processing Order ID: " << currentOrder.getOrderId() 
             << " - " << currentOrder.getItemName() 
             << " - Burst Time: " << currentOrder.getPrepTime() << " minutes" << endl;
        
        sleep_for(seconds(currentOrder.getPrepTime())); // Simulate processing
        orderQueue.pop_front();
        cout << "Completed Order ID: " << currentOrder.getOrderId() << endl;
        cout << "Remaining in queue is " << getQueueSize() << endl;
        orderQueue.pop_front();
    }
}

void FCFS::start() {
    static bool started = false;
    if (started) return; // Prevent starting multiple threads
    started = true;
    std::thread processingThread(&FCFS::processOrders, this);
    processingThread.detach(); // Run in background
}

void FCFS::stop() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        isProcessing = false;
    }
    cv.notify_all(); // wake any waiting threads so they can exit
}

size_t FCFS::getQueueSize() {
    std::lock_guard<std::mutex> lock(queueMutex);
    return orderQueue.size();
}

std::deque<Order> FCFS::getQueue() {
    return orderQueue;
}