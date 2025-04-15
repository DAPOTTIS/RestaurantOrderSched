#ifndef PQ_H
#define PQ_H

#include <vector>
#include<mutex>
#include<thread>
#include<condition_variable>
#include"Order.h"
#include"Menu.h"

// struct Order {
//     int priority;
//     int orderID;
// };

class PQ {
private:
    static std::vector<Order> heap;
    static std::mutex heapMutex;
    static std::condition_variable cv;
    static bool isProcessing;

    int parent(int i);
    int left(int i);
    int right(int i);

    void heapifyUp(int index);
    void heapifyDown(int index);


public:
    bool empty() const;
    void addOrder(const Order& order);
    void addOrder(const Menu& menu);
    void processOrders();
    void start();
    void stop();

    std::vector<Order> getHeap();
    size_t getHeapSize();
    Order top() const;
    Order pop();

};

#endif //PQ_H
