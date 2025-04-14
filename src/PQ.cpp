#include <iostream>
#include <queue>
#include <vector>
#include <stdexcept>
#include "PQ.h"
#include "Menu.h"

int PQ::parent(int i) {
    return (i - 1) / 2;
}

int PQ::left(int i) {
    return (2 * i + 1);
}

int PQ::right(int i) {
    return (2 * i + 2);
}

void PQ::heapifyUp(int index) {
    // using this function to make the node with the highest priority the root node
    while (index > 0 && heap[index].priority > heap[parent(index)].priority) {
        std::swap(heap[index], heap[parent(index)]);
        // swapping both orders
        index = parent(index);
        // to check the parent node's priority and placement in the tree
    }
}

void PQ::heapifyDown(int index) {
    // use this to check the priority order of the PQ
    int n = heap.size();

    while (true) {
        int l = left(index);
        int r = right(index);
        int largest = index;

        if (l < n && heap[l].priority > heap[index].priority)
            largest = l;
        // to check if the left of the index is greater
        if (r < n && heap[r].priority > heap[index].priority)
            largest = r;
        if (largest != index) {
            std::swap(heap[index], heap[largest]);
            index = largest;
        } else {
            break;
            // if the queue is in order break
        }
    }
}

bool PQ::empty() const {
    return heap.empty();
}

void PQ::insert(const Order& order) {
    heap.push_back(order);
    heapifyUp(heap.size() - 1);
}

Order PQ::top() const {
    if (empty())
        throw std::runtime_error("PriorityQueue is empty!");
    return heap.front();
}

Order PQ::pop() {
    if (empty())
        throw std::runtime_error("PriorityQueue is empty!");
    Order popped = heap[0];
    std::swap(heap[0], heap.back());
    heap.pop_back();
    heapifyDown(0);
    return popped;
}
