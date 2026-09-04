#pragma once
#include <string>
class Player;

class Item
{
public:
    std::string name;
    std::string type;
    std::string description;
    Item(std::string n, std::string t, std::string d);
    virtual void use(Player* p) = 0; // 纯虚函数
};

class Weapon
{
public:
    std::string name;
    int damage;
    Weapon(std::string n, int d);
    virtual void use(Player* p) = 0;
};
