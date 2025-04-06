#ifndef ORDER_H
#define ORDER_H

enum Priority {
    VIP,
    NORMAL
};

class Order {
public:
    int burstTime;
    int arrivalTime;
    int pid;
    int waitingTime = 0;

    Priority priority;

    int getPrepTime() const {return burstTime;}
    int getArrivalTime() const {return arrivalTime;}
    void setPrepTime(const int prep) {burstTime = prep;}
    void setArrivalTime(const int arrival) {arrivalTime = arrival;}

    Order(int pid, int prepTime, int arrivalTime, Priority priority) :
    pid(pid), burstTime(prepTime), arrivalTime(arrivalTime), priority(priority){}
};



#endif //ORDER_H
