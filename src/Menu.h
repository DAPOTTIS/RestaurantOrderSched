#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include "MenuItem.h"
using namespace std;

class Menu
{
private:
    int id;
    string name;
    vector<MenuItem> items;
public:
    Menu(string name);
    void addItem(const MenuItem& item);
    const vector<MenuItem>& getItems() const;
    MenuItem getItemById(int id) const;
    string getName() const;
    int getItemsCount() const;
    void display() const;
};

#endif MENU_H