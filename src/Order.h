#include "States.h"
#include "MenuItem.h"
#include <string>
#ifndef ORDER_H
#define ORDER_H

#include <chrono>
using namespace std::chrono;



enum Priority {
    VIP,
    NORMAL
};

class Order {
private:
    int remainingTime; // for RR
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
    int pid; // Process ID
    ProcessState state;
    Priority priority;
    int arrivalTime; // Arrival time
    int waitingTime = 0; // Waiting time

    Order() : pid(0), state(ProcessState::NEW), priority(NORMAL), arrivalTime(0) {}
    Order(const MenuItem& mItem, int pID, Priority p) : item(mItem), pid(pID), state(ProcessState::NEW), priority(NORMAL), arrivalTime(0) {}
    int getPrepTime() const { return item.burstTime; }
    int getArrivalTime() const { return arrivalTime; } 
    int getOrderId() const { return pid; }
    std::string getItemName() const { return item.name; }




    Order(const MenuItem& mItem, int pID, Priority p, int arrival)
    : item(mItem), pid(pID), state(ProcessState::NEW), priority(p),
      arrivalTime(arrival), remainingTime(mItem.burstTime) {}


    int getRemainingTime() const {
        return remainingTime;
    }

    void reduceRemainingTime(int time) {
        remainingTime -= time;
        if (remainingTime < 0) remainingTime = 0;
    }
};



#endif //ORDER_H
