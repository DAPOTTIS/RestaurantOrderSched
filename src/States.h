#ifndef STATES_H
#define STATES_H

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

void admitProcess();
void dispatchProcess(); 
void setWaitingState();
void terminateProcess();
void runProcess();

#endif //STATES_H