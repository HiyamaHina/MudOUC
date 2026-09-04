#include "enemyevent.h"
#include "player.h"
#include <iostream>

Enemy::Enemy(std::string n, int h, int a)
    : name(n), hp(h), attack(a)
{}

// ✅函数名改成 doAttack
void Enemy::doAttack(Player* p)
{
    p->setHp(p->getHp() - attack);
    std::cout << name << "对你发起攻击！你受到" << attack << "点伤害！" << std::endl;
}

Event::Event(std::string n, std::string d)
    : name(n), description(d)
{}
