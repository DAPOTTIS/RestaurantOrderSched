//
// Created by engah on 4/13/2025.
//

#include "RR.h"
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
int RR::timeQuantum = 6;     //quantum is 6 to avoid high context switch overhead

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
            this->stop();
            exit(0);
        }
        if (x < 1 || x > static_cast<int>(menu.getItemsCount())) {
            cout << "Invalid item ID. Try again.\n";
            continue;
        }
        const auto& item = menu.getItemById(x);
        this->addOrder(Order(item, ++id, NORMAL));
    }
}

void RR::processOrders() {
    isProcessing = true;
    size_t index = 0;
    while (isProcessing) {
        unique_lock<mutex> lock(listMutex);
        cv.wait(lock, []() { return !orderList.empty() || !isProcessing; });

        if (!isProcessing && orderList.empty()) break;
        if (orderList.empty()) continue;

        if (index >= orderList.size()) index = 0;

        Order& currentOrder = orderList[index];
        int runTime = min(timeQuantum, currentOrder.getRemainingTime());

        lock.unlock(); // Unlock while sleeping to not block addOrder()
        sleep_for(milliseconds(1)); // Avoid print overlap
        cout << "Processing Order ID: " << currentOrder.getOrderId()
             << " - " << currentOrder.getItemName()
             << " for " << runTime << " minutes. Remaining: "
             << currentOrder.getRemainingTime() - runTime << " minutes.\n";

        sleep_for(seconds(runTime));

        lock.lock();
        currentOrder.reduceRemainingTime(runTime);
        if (currentOrder.getRemainingTime() <= 0) {
            cout << "Completed Order ID: " << currentOrder.getOrderId() << endl;
            orderList.erase(orderList.begin() + index); // remove from vector
             // don't increment index because next item shifted left
        } else {
            index++;
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
