#include <iostream>
#include <queue>
#include <vector>
#include <stdexcept>
#include "PQ.h"
#include "Menu.h"

bool PQ::isProcessing = false;
std::vector<Order> PQ::heap;
std::condition_variable PQ::cv;
std::mutex PQ::heapMutex;

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
    while (index > 0 && heap[index].priority >= heap[parent(index)].priority) {
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

        if (l < n && heap[l].priority >= heap[index].priority)
            largest = l;
        // to check if the left of the index is greater
        if (r < n && heap[r].priority >= heap[index].priority)
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

void PQ::addOrder(const Order& order) {
    {
        std::lock_guard<std::mutex> lock(heapMutex);

        heap.push_back(order);
        heapifyUp(heap.size() - 1);
    }
    cv.notify_one();
    cout << "New order added! Current in queue is " << getHeapSize() << endl;
}

void PQ::addOrder(const Menu &menu) {
    while(true){
        int x = -1;
        this_thread::sleep_for(std::chrono::milliseconds(2)); // Avoid print conflict
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
        int p;
        cout<<"\nPlease enter the priority (0 or 1):\n";
        cin>>p;

        const auto& item = menu.getItemById(x);
        this->addOrder(Order(item, static_cast<Priority>(p)));
    }
}

void PQ::processOrders() {
    isProcessing =true;
    while (isProcessing) {
        Order currentOrder;
        {
            std::unique_lock<std::mutex> lock(heapMutex);
            cv.wait(lock,[this](){return !heap.empty()||!isProcessing;});
            if (!isProcessing&&heap.empty()) break;
            currentOrder = heap.front();
        }

        this_thread::sleep_for(std::chrono::milliseconds(1)); // Avoid print conflict
        cout << "Processing Order ID: " << currentOrder.getOrderId()
             << " - " << currentOrder.getItemName()
             << " - Burst Time: " << currentOrder.getPrepTime() << " minutes" << endl;

        this_thread::sleep_for(seconds(currentOrder.getPrepTime())); // Simulate processing
        cout << "Completed Order ID: " << currentOrder.getOrderId() << endl;
        cout << "Remaining in queue is " << getHeapSize() << endl;
        pop();
    }
}

void PQ::start() {
    static bool started= false;
    if (started) return;
    started= true;
    std::thread processingThread(&PQ::processOrders, this);
    processingThread.detach();
}

void PQ::stop() {
    {
        std::lock_guard<std::mutex> lock(heapMutex);
        isProcessing = false;
    }
    cv.notify_all(); // wake any waiting threads so they can exit
}


std::vector<Order> PQ::getHeap() {
    return heap;
}

size_t PQ::getHeapSize() {
    std::lock_guard<std::mutex> lock(heapMutex);

    return heap.size();
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
