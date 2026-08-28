#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(std::string name)
    : name(name),
      level(1),
      exp(0),
      expToNextLevel(100),
      maxHp(100),
      hp(100),
      shield(0),
      baseAttack(10),
      equippedWeapon(), // 默认武器"拳头"，攻击加成0
      gold(0)
{
}

std::string Player::getName() const {
    return name;
}

// ---------------- 等级与经验 ----------------
int Player::getLevel() const {
    return level;
}

int Player::getExp() const {
    return exp;
}

void Player::gainExp(int amount) {
    exp += amount;
    std::cout << name << " 获得了 " << amount << " 点经验值。\n";
    while (exp >= expToNextLevel) {
        exp -= expToNextLevel;
        levelUp();
    }
}

void Player::levelUp() {
    level++;
    maxHp += 20;
    hp = maxHp;
    baseAttack += 5;
    expToNextLevel += 50;
    std::cout << "*** " << name << " 升级了！当前等级：" << level << " ***\n";
}

// ---------------- 生命与护盾 ----------------
int Player::getHp() const {
    return hp;
}

int Player::getMaxHp() const {
    return maxHp;
}

int Player::getShield() const {
    return shield;
}

void Player::takeDamage(int amount) {
    if (amount <= 0) return;

    if (shield > 0) {
        int absorbed = std::min(shield, amount);
        shield -= absorbed;
        amount -= absorbed;
    }

    hp -= amount;
    if (hp < 0) hp = 0;

    std::cout << name << " 受到了伤害，剩余生命：" << hp << "/" << maxHp << "\n";
}

void Player::heal(int amount) {
    if (amount <= 0) return;
    hp = std::min(hp + amount, maxHp);
    std::cout << name << " 恢复了生命，当前生命：" << hp << "/" << maxHp << "\n";
}

void Player::addShield(int amount) {
    if (amount <= 0) return;
    shield += amount;
    std::cout << name << " 获得了 " << amount << " 点护盾，当前护盾：" << shield << "\n";
}

bool Player::isAlive() const {
    return hp > 0;
}

// ---------------- 攻击力 ----------------
int Player::getAttack() const {
    return baseAttack + equippedWeapon.attackBonus;
}

// ---------------- 金币 ----------------
int Player::getGold() const {
    return gold;
}

void Player::addGold(int amount) {
    if (amount <= 0) return;
    gold += amount;
    std::cout << name << " 获得了 " << amount << " 金币，当前金币：" << gold << "\n";
}

bool Player::spendGold(int amount) {
    if (amount > gold) {
        std::cout << "金币不足！\n";
        return false;
    }
    gold -= amount;
    return true;
}

// ---------------- 背包（消耗品）----------------
void Player::addItem(const Item& item) {
    bag.push_back(item);
    std::cout << name << " 获得了物品：" << item.name << "\n";
}

bool Player::removeItem(const std::string& itemName) {
    for (auto it = bag.begin(); it != bag.end(); ++it) {
        if (it->name == itemName) {
            bag.erase(it);
            return true;
        }
    }
    return false;
}

const std::vector<Item>& Player::getBag() const {
    return bag;
}

// ---------------- 武器 ----------------
void Player::equipWeapon(const Weapon& weapon) {
    equippedWeapon = weapon;
    std::cout << name << " 装备了武器：" << weapon.name
               << "（攻击加成+" << weapon.attackBonus << "）\n";
}

const Weapon& Player::getWeapon() const {
    return equippedWeapon;
}

// ---------------- 状态显示 ----------------
void Player::printStatus() const {
    std::cout << "========== 角色状态 ==========\n";
    std::cout << "姓名：" << name << "\n";
    std::cout << "等级：" << level << "  经验：" << exp << "/" << expToNextLevel << "\n";
    std::cout << "生命：" << hp << "/" << maxHp << "  护盾：" << shield << "\n";
    std::cout << "攻击力：" << getAttack() << "（基础" << baseAttack << "）\n";
    std::cout << "金币：" << gold << "\n";
    std::cout << "武器：" << equippedWeapon.name
               << "（等级" << equippedWeapon.level
               << "，加成+" << equippedWeapon.attackBonus << "）\n";
    std::cout << "背包物品数：" << bag.size() << "\n";
    std::cout << "==============================\n";
}
