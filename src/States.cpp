#include <iostream>
#include "States.h"


enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

void admitProcess(){
    // Code to admit a process
    std::cout << "Process admitted." << std::endl;
}

void dispatchProcess(){
    // Code to dispatch a process
    std::cout << "Process dispatched." << std::endl;
}

void setWaitingState(){
    // Code to wait for a process
    std::cout << "Process waiting." << std::endl;
}
void terminateProcess(){
    // Code to terminate a process
    std::cout << "Process terminated." << std::endl;
}
void runProcess(){
    // Code to run a process
    std::cout << "Process running." << std::endl;
}