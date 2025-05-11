#include "Timer.h"

double Timer::totalWaitTime = 0.0; // Initialize static member

std::vector<double> Timer::totalWaitTimes = {0.0, 0.0, 0.0}; // Initialize static member

// In Timer.cpp
// ... other initializations ...
double Timer::totalTurnaroundTime = 0.0;
std::vector<double> Timer::totalTurnaroundTimes(3, 0.0); // Initialize static member for turnaround times