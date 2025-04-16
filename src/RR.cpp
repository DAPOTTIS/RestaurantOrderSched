//
// Created by engah on 4/13/2025.
//

#include "RR.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

vector<Order> RR::orderList;
mutex RR::listMutex;
condition_variable RR::cv;
bool RR::isProcessing = false;
// int RR::id = 0;
int RR::timeQuantum = 8;         // Most jobs are around 10 mins avg, this lets short orders
                                // finish quickly, while rotating the longer ones fairly

RR::RR() {}

RR::RR(int quantum) {
    timeQuantum = quantum;
}

void RR::addOrder(const Order& order) {
    {
        lock_guard<mutex> lock(listMutex);
        orderList.push_back(order);
    }
    cv.notify_one();
    cout << "New order added! Total orders: " << getOrderCount() << endl;
}

void RR::addOrder(const Menu& menu) {
    while (true) {
        int x = -1;
        sleep_for(milliseconds(2));
        menu.display();
        cout << "\nEnter item ID to order (0 to exit): ";
        cin >> x;

        if (x == 0) {
            this->stop(); // Stop processing loop
            exit(0);
        }

        if (x < 1 || x > static_cast<int>(menu.getItemsCount())) {
            cout << "Invalid item ID. Try again.\n";
            continue;
        }

        const auto& item = menu.getItemById(x);
        // Order newOrder(item, ++id, NORMAL); //Leaving the mistake to be seen
        // Order newOrder(item);

        // remaining time is initialized to burst time
        // REPLY: This is already handled in the Order constructor
        // newOrder.setRemainingTime(item.burstTime);

        // {
        //     lock_guard<mutex> lock(listMutex);
        //     orderList.push_back(newOrder);
        //     cout << "New order added! Current in queue is " << orderList.size() << endl;
        // }

        // cv.notify_one(); // Wake the processing thread
        this->addOrder(Order(item)); // Add order to the list using the new method
    }
}


void RR::processOrders() {
    isProcessing = true;
    while (isProcessing) {
        unique_lock<mutex> lock(listMutex);
        cv.wait(lock, [this] { return !orderList.empty() || !isProcessing; });
        if (!isProcessing && orderList.empty()) break;
        
        Order currentOrder = orderList.front();
        orderList.erase(orderList.begin());
        lock.unlock();
        
        int runTime = min(timeQuantum, currentOrder.getRemainingTime());
        cout << "\n[RR] Processing Order ID: " << currentOrder.getOrderId()
             << " - " << currentOrder.getItemName()
             << " for " << runTime << " minutes (Remaining before: "
             << currentOrder.getRemainingTime() << ")\n";
        
        sleep_for(seconds(runTime)); // Simulate processing time
        
        currentOrder.reduceRemainingTime(runTime);
        lock.lock();
        if (currentOrder.getRemainingTime() > 0) {
            orderList.push_back(currentOrder);
        } else {
            cout << "[RR] Completed Order ID: " << currentOrder.getOrderId() << "\n";
        }
    }
}



void RR::start() {
    static bool started = false;
    if (started) return;
    started = true;
    thread t(&RR::processOrders, this);
    t.detach();
}

void RR::stop() {
    {
        lock_guard<mutex> lock(listMutex);
        isProcessing = false;
    }
    cv.notify_all();
}

size_t RR::getOrderCount() {
    lock_guard<mutex> lock(listMutex);
    return orderList.size();
}
