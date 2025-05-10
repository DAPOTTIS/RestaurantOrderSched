#include "Timer.h"

double Timer::totalWaitTime = 0.0; // Initialize static member

std::vector<double> Timer::totalWaitTimes = {0.0, 0.0, 0.0}; // Initialize static member