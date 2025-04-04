// filepath: c:\Users\Ahmed Essam\development\C\RestaurantOrderSched\src\menu.cpp
#include "Menu.h"
#include "MenuItem.h"
#include <iostream>
#include <iomanip>
using namespace std;

// Constructor
Menu::Menu(string name) : name(name) {}

// Add an item to the menu
void Menu::addItem(const MenuItem& item) {
    items.push_back(item);
}

// Get all items in the menu
const vector<MenuItem>& Menu::getItems() const {
    return items;
}

// Get the name of the menu
string Menu::getName() const {
    return name;
}

// Display the menu
void Menu::display() const {
    cout << "Menu: " << name << endl;
    cout << "--------------------------------------------------" << endl;
    cout << "| ID   | Name             | Price   | Burst Time |" << endl;
    cout << "--------------------------------------------------" << endl;
    for (const auto& item : items) {
        cout << "| " << setw(4) << item.id 
             << " | " << setw(16) << left << item.name 
             << " | " << setw(7) << right << fixed << setprecision(2) << item.price 
             << " | " << setw(10) << item.burstTime << " |" << endl;
    }
    cout << "--------------------------------------------------" << endl;
}

Menu createEgyptianMenu() {
    Menu egyptian("Egyptian Menu");
    egyptian.addItem(MenuItem(1, "Pita Bread", 4, 5));
    egyptian.addItem(MenuItem(2, "Falafel Wrap", 7, 10));
    egyptian.addItem(MenuItem(3, "Koshari", 12, 15));
    egyptian.addItem(MenuItem(4, "Chicken Shawarma", 10, 12));
    egyptian.addItem(MenuItem(5, "Hummus Dip", 5, 8));
    egyptian.addItem(MenuItem(6, "Aish Baladi", 8, 6));
    egyptian.addItem(MenuItem(7, "Basbousa", 15, 20));
    egyptian.addItem(MenuItem(8, "Molokhia Soup", 9, 18));
    egyptian.addItem(MenuItem(9, "Taameya", 6, 10));
    egyptian.addItem(MenuItem(10, "Grilled Kofta", 14, 25));
    return egyptian;
}
