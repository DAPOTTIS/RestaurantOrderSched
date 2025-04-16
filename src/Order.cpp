//
// Created by ms on 01/04/25.
//

#include "Order.h"

int Order::orderCount = 0; // Initialize static variable

void Order::reduceRemainingTime(int time) {
    remainingTime -= time;
    if (remainingTime < 0) remainingTime = 0;
}

void Order::setRemainingTime(int time) {
    remainingTime = time;
