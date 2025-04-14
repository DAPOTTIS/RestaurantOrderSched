#ifndef PQ_H
#define PQ_H

#include <vector>

struct Order {
    int priority;
    int orderID;
};

class PQ {
private:
    std::vector<Order> heap;

    int parent(int i);
    int left(int i);
    int right(int i);

    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    bool empty() const;
    void insert(const Order& order);
    Order top() const;
    Order pop();
};

#endif //PQ_H
