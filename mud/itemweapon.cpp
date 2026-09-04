#include "ItemWeapon.h"
#include "Player.h"

Item::Item(std::string n, std::string t, std::string d)
    : name(n), type(t), description(d)
{}

Weapon::Weapon(std::string n, int d)
    : name(n), damage(d)
{}
