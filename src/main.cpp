#include <iostream>
#include "Menu.h"

int main(){
    std::cout << "hi";
    extern Menu createEgyptianMenu();
    Menu egyptianMenu = createEgyptianMenu();
    egyptianMenu.display();
}