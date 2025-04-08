#ifndef FCFS_H
#define FCFS_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Order.h"
#include "Menu.h"

using namespace std;

class FCFS {
private:
    std::queue<Order> orderQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool isProcessing;
    static int id; // Static variable to keep track of the order ID

public:
    FCFS();
    
    void addOrder(const Order& order);
    void addOrder(const Menu& menu);
    void processOrders();
    void start();
    void stop();
    size_t getQueueSize();
};

#endif // FCFS_H