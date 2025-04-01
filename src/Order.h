#ifndef ORDER_H
#define ORDER_H

enum Priority {
    VIP,
    NORMAL
};

class Order {
private:
    int prepTime;
    int arrivalTime;
    Priority priority;
public:
    int getPrepTime() const {return prepTime;}
    int getArrivalTime() const {return arrivalTime;}
    void setPrepTime(const int prep) {prepTime = prep;}
    void setArrivalTime(const int arrival) {arrivalTime = arrival;}

    Order(int prepTime, int arrivalTime, Priority priority) :
    prepTime(prepTime), arrivalTime(arrivalTime), priority(priority){}
};



#endif //ORDER_H
