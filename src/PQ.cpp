#include "PQ.h"
#include "Order.h" 
#include "Menu.h"  

#include <iostream>
#include <chrono> 
#include <thread> 
#include <algorithm> // For std::min
#include <optional>  // For std::optional
#include <vector>    // For std::vector

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

// Static member definitions
std::deque<Order> PriorityScheduler::highPriorityQueue;
std::deque<Order> PriorityScheduler::lowPriorityQueue;
std::mutex PriorityScheduler::queueMutex; // Protects the two deques above
std::condition_variable PriorityScheduler::cv;
bool PriorityScheduler::isProcessing = false;
bool PriorityScheduler::threadStarted = false;

// New static members for currently processing order
std::optional<Order> PriorityScheduler::s_currentlyProcessingOrder;
std::mutex PriorityScheduler::s_currentOrderMutex; // Protects s_currentlyProcessingOrder

// Static member definitions for completed orders
std::vector<Order> PriorityScheduler::s_completedOrders;
std::mutex PriorityScheduler::s_completedOrdersMutex;

PriorityScheduler::PriorityScheduler() {
    // Constructor
}

// New public static method
std::optional<Order> PriorityScheduler::getCurrentlyProcessingOrder() {
    std::lock_guard<std::mutex> lock(s_currentOrderMutex);
    return s_currentlyProcessingOrder;
}

void PriorityScheduler::addOrder(const Order& order) {
    std::string queueType;
    {
        std::lock_guard<std::mutex> lock(queueMutex); // queueMutex for the deques
        if (order.getPrepTime() < PRIORITY_THRESHOLD) {
            highPriorityQueue.push_back(order);
            queueType = "High Priority";
        } else {
            lowPriorityQueue.push_back(order);
            queueType = "Low Priority";
        }
    }
    cv.notify_one(); 
}

void PriorityScheduler::addOrder(const Menu& menu) {
    // ... (addOrder from menu remains the same)
    while (true) {
        int itemId = -1;
        sleep_for(milliseconds(100)); 
        menu.display();
        cout << "\nEnter item ID to order for Priority Scheduler (0 to stop & exit): ";
        cin >> itemId;

        if (cin.fail()) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            continue;
        }

        if (itemId == 0) {
            this->stop(); 
            cout << "Exiting order placement. Processing will stop if no orders are left." << endl;
            return; 
        }

        if (itemId < 1 || itemId > static_cast<int>(menu.getItemsCount())) {
            cout << "Invalid item ID. Please try again.\n";
            continue;
        }
        
        const auto& item = menu.getItemById(itemId); 
        this->addOrder(Order(item)); 
    }
}

void PriorityScheduler::processOrders() {
    PriorityScheduler::isProcessing = true;

    while (true) {
        Order localCurrentOrder; // Use a local variable for the order being processed in this iteration
        bool orderFound = false;
        // bool currentOrderIsHighPriority = false; // Not strictly needed outside the picking scope

        { 
            std::unique_lock<std::mutex> lock(queueMutex); // queueMutex for deques
            cv.wait(lock, [] { 
                return (!highPriorityQueue.empty() || !lowPriorityQueue.empty()) || !PriorityScheduler::isProcessing;
            });

            if (!PriorityScheduler::isProcessing && highPriorityQueue.empty() && lowPriorityQueue.empty()) {
                break; 
            }

            if (!highPriorityQueue.empty()) {
                localCurrentOrder = highPriorityQueue.front();
                highPriorityQueue.pop_front(); // POPPED HERE
                orderFound = true;
                // currentOrderIsHighPriority = true;
            } else if (!lowPriorityQueue.empty()) {
                localCurrentOrder = lowPriorityQueue.front();
                lowPriorityQueue.pop_front(); // POPPED HERE
                orderFound = true;
                // currentOrderIsHighPriority = false;
            } else {
                continue; 
            }
        } 

        if (orderFound) {
            { // Set currently processing order
                std::lock_guard<std::mutex> currentLock(s_currentOrderMutex);
                s_currentlyProcessingOrder = localCurrentOrder;
            }

            bool isHighPriority = localCurrentOrder.getPrepTime() < PRIORITY_THRESHOLD; // Determine priority again if needed, or pass from above

            if (isHighPriority) {
                int timeToRun = localCurrentOrder.getRemainingTime();
                if (timeToRun > 0) { 
                    sleep_for(seconds(timeToRun));
                    // localCurrentOrder.reduceRemainingTime(timeToRun); // Order is modified, but it's a copy.
                                                                      // If it were to be re-queued for RR like behavior within high-prio, this would be needed.
                                                                      // For simple run-to-completion, this line isn't strictly necessary if not re-queued.
                }
                // Order completed
                { // Add to completed orders
                    std::lock_guard<std::mutex> lock(s_completedOrdersMutex);
                    s_completedOrders.push_back(localCurrentOrder);
                }
            } else { // Low-priority order
                bool preempted = false;
                while (localCurrentOrder.getRemainingTime() > 0) { // Process the local copy
                    { 
                        std::lock_guard<std::mutex> checkLock(queueMutex); // queueMutex for deques
                        if (!highPriorityQueue.empty()) {
                            preempted = true;
                            break; 
                        }
                    }

                    int timeQuantum = 4; // Using a slightly larger quantum for low-prio
                    int runTime = std::min(timeQuantum, localCurrentOrder.getRemainingTime());
                    sleep_for(seconds(runTime));
                    localCurrentOrder.reduceRemainingTime(runTime); // Modify the local copy

                    if (localCurrentOrder.getRemainingTime() == 0) {
                        break; 
                    }
                } 

                if (preempted) {
                    std::lock_guard<std::mutex> requeueLock(queueMutex); // queueMutex for deques
                    lowPriorityQueue.push_front(localCurrentOrder); // Put the (modified local) order back
                } else if (localCurrentOrder.getRemainingTime() == 0) {
                    // Order completed
                    { // Add to completed orders
                        std::lock_guard<std::mutex> lock(s_completedOrdersMutex);
                        s_completedOrders.push_back(localCurrentOrder);
                    }
                }
            }

            { // Clear currently processing order
                std::lock_guard<std::mutex> currentLock(s_currentOrderMutex);
                s_currentlyProcessingOrder.reset();
            }
        }
    } 
}

void PriorityScheduler::start() {
    std::lock_guard<std::mutex> lock(queueMutex); 
    if (PriorityScheduler::threadStarted) {
        return;
    }
    PriorityScheduler::threadStarted = true;
    
    std::thread processingThread(&PriorityScheduler::processOrders, this);
    processingThread.detach(); 
}

void PriorityScheduler::stop() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        isProcessing = false; 
    }
    cv.notify_all(); 
}

size_t PriorityScheduler::getOrderCount() const {
    std::lock_guard<std::mutex> lock(queueMutex); // queueMutex for deques
    return highPriorityQueue.size() + lowPriorityQueue.size();
}

// These get the "upcoming" orders
std::deque<Order> PriorityScheduler::getHighPriorityQueueCopy() const {
    std::lock_guard<std::mutex> lock(queueMutex); // queueMutex for deques
    return highPriorityQueue; 
}

std::deque<Order> PriorityScheduler::getLowPriorityQueueCopy() const {
    std::lock_guard<std::mutex> lock(queueMutex); // queueMutex for deques
    return lowPriorityQueue; 
}

// New public static method to get completed orders
std::vector<Order> PriorityScheduler::getCompletedOrders() {
    std::lock_guard<std::mutex> lock(s_completedOrdersMutex);
    return s_completedOrders;
}
