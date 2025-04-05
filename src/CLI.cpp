#include "CLI.h"
#include "iostream"
#include "Menu.h"


void CLI::init() {
    std::cout << "hi";
    extern Menu createEgyptianMenu();
    Menu egyptianMenu = createEgyptianMenu();
    egyptianMenu.display();
}