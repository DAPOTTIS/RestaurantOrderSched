#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
using namespace std;

struct MenuItem
{
    int id;
    string name;
    int price;
    int burstTime;

    MenuItem(int itemId, const string& itemName, int itemPrice, int itemBurstTime)
        : id(itemId), name(itemName), price(itemPrice), burstTime(itemBurstTime) {}
};

#endif MENUITEM_H
