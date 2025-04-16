//
// Created by engah on 4/13/2025.
//

#ifndef RR_H
#define RR_H

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <Order.h>
#include <Menu.h>

using namespace std;

class RR {
private:
    static vector<Order> orderList;
    static mutex listMutex;
    static bool isProcessing;
    // static int id;
    static int timeQuantum;
    static condition_variable cv;
    static int currentTime;
    // int nextOrderId = 1;

public:
    RR();
    RR(int quantum);
    void addOrder(const Order& order);
    void addOrder(const Menu& menu);
    void processOrders();
    void start();
    void stop();
    size_t getOrderCount();
};




#endif //RR_H
