#include "States.h"
#include "MenuItem.h"
#include <string>
#ifndef ORDER_H
#define ORDER_H



enum Priority {
    VIP,
    NORMAL
};

class Order {
public:
    // std::string itemName;
    // int burstTime;
    // int arrivalTime;
    // int pid;
    // int waitingTime = 0;
    // ProcessState state;

    // Priority priority;

    // int getPrepTime() const {return burstTime;}
    // int getArrivalTime() const {return arrivalTime;}
    // void setPrepTime(const int prep) {burstTime = prep;}
    // void setArrivalTime(const int arrival) {arrivalTime = arrival;}
    // int getOrderId() const {return pid;}
    // std::string getItemName() const {return itemName;}


    // Order(int pid, int prepTime, int arrivalTime, Priority priority, std::string itemName) :
    // pid(pid), burstTime(prepTime), arrivalTime(arrivalTime), priority(priority), itemName(itemName){}

    // Order() : pid(0), burstTime(0), arrivalTime(0), priority(NORMAL), itemName(""), state(ProcessState::NEW) {}

    MenuItem item; // Menu item
    static int orderCount;
    int pid; // Process ID
    ProcessState state;
    Priority priority;
    int arrivalTime; // Arrival time
    int waitingTime = 0; // Waiting time

    Order() : pid(0), state(ProcessState::NEW), priority(NORMAL), arrivalTime(0) {}
    Order(const MenuItem& mItem, Priority p = NORMAL) : item(mItem), pid(++orderCount), state(ProcessState::NEW), priority(p), arrivalTime(0) {}
    Order(const MenuItem& mItem, int pID, Priority p) : item(mItem), pid(pID), state(ProcessState::NEW), priority(NORMAL), arrivalTime(0) {}
    int getPrepTime() const { return item.burstTime; }
    int getArrivalTime() const { return arrivalTime; } 
    int getOrderId() const { return pid; }
    std::string getItemName() const { return item.name; }
};



#endif //ORDER_H
