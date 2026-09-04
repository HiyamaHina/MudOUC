#include "Player.h"
#include "ItemWeapon.h"
#include <iostream>

Player::Player()
{
    hp = 100;
    coin = 0;
}

void Player::addWeapon(Weapon* w)
{
    weapons.push_back(w);
}

void Player::addItem(Item* it)
{
    items.push_back(it);
}

void Player::removeItem(Item* it)
{
    for (auto iter = items.begin(); iter != items.end(); ++iter)
    {
        if (*iter == it)
        {
            items.erase(iter);
            break;
        }
    }
}

void Player::showStatus()
{
    std::cout << "\n===== 你的状态 =====" << std::endl;
    std::cout << "血量：" << hp << std::endl;
    std::cout << "选课币：" << coin << std::endl;
    std::cout << "背包道具：";
    for (auto i : items) std::cout << i->name << " ";
    std::cout << "\n武器：";
    for (auto w : weapons) std::cout << w->name << " ";
    std::cout << "\n=====================\n" << std::endl;
}
