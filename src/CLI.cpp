#include "CLI.h"
#include "iostream"
#include "Menu.h"
#include "FCFS.h"
#include "RR.h"


void CLI::init() {
    std::cout << "hi";

    Menu menu("Egyptian Menu"); // Create a menu object
    menu = menu.createEgyptianMenu(); // Create the Egyptian menu

    // static FCFS scheduler; // Made static to extend lifetime for the processing thread
    // scheduler.start();
    // scheduler.addOrder(menu); // Add orders from the menu

    // static RR schedulerRR(6); // Create a Round Robin scheduler with a time quantum of 6
    // schedulerRR.start();
    // schedulerRR.addOrder(menu); // Add orders from the menu


    // Uncomment the following lines to add specific orders with delays
    
    // scheduler.addOrder(Order(items[0], 1, NORMAL));
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    // scheduler.addOrder(Order(items[5], 2, NORMAL));
    // std::this_thread::sleep_for(std::chrono::seconds(30));
    // scheduler.addOrder(Order(items[1], 3, NORMAL)); 
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // scheduler.addOrder(Order(items[8], 4, NORMAL)); 
    // std::this_thread::sleep_for(std::chrono::seconds(7));
    // scheduler.addOrder(Order(items[6], 5, NORMAL));
    // scheduler.stop(); // Stop processing orders



    //static RR scheduler(8);
    //scheduler.start();
    //scheduler.addOrder(menu);


}