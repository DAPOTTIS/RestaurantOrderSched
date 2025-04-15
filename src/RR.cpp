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
int RR::id = 0;
int RR::timeQuantum = 8;         // Most jobs are around 10 mins avg, this lets short orders
                                // finish quickly, while rotating the longer ones fairly

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
        Order newOrder(item, ++id, NORMAL);

        // remaining time is initialized to burst time
        newOrder.setRemainingTime(item.burstTime);

        {
            lock_guard<mutex> lock(listMutex);
            orderList.push_back(newOrder);
            cout << "New order added! Current in queue is " << orderList.size() - 1 << endl;
        }

        cv.notify_one(); // Wake the processing thread
    }
}


void RR::processOrders() {
    isProcessing = true;
    size_t index = 0;

    while (isProcessing) {
        unique_lock<mutex> lock(listMutex);
        cv.wait(lock, [] { return !orderList.empty() || !isProcessing; });

        if (!isProcessing && orderList.empty()) break;
        if (orderList.empty()) continue;

        // Reset index if it's out of bounds
        if (index >= orderList.size()) index = 0;

        Order& currentOrder = orderList[index];

        int runTime = min(timeQuantum, currentOrder.getRemainingTime());

        cout << "\n[RR] Processing Order ID: " << currentOrder.getOrderId()
             << " - " << currentOrder.getItemName()
             << " for " << runTime << " minutes (Remaining before: "
             << currentOrder.getRemainingTime() << ")\n";

        lock.unlock(); // Allow order addition while simulating processing
        sleep_for(seconds(runTime)); // Simulate time quantum

        lock.lock(); // Lock again to safely update order list

        // Making sure the order hasn't been removed by another thread
        if (index < orderList.size()) {
            currentOrder.reduceRemainingTime(runTime);

            if (currentOrder.getRemainingTime() <= 0) {
                cout << "[RR] Completed Order ID: " << currentOrder.getOrderId() << "\n";
                orderList.erase(orderList.begin() + index);
                // Don't increment index since the vector shifted
            } else {
                // This order still has work to do, move to the next one
                index++;

                // If we've reached the end, start over from the beginning
                if (index >= orderList.size()) {
                    index = 0;
                }
            }
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
