#pragma once
#include <string>
class Player;

class Enemy
{
public:
    std::string name;
    int hp;
    int attack;   // 攻击力变量保留
    Enemy(std::string n, int h, int a);
    void doAttack(Player* p);  // ✅改名 doAttack，不再和变量attack冲突
};

class Event
{
public:
    std::string name;
    std::string description;
    Event(std::string n, std::string d);
    virtual void trigger(Player* p) = 0;
};
