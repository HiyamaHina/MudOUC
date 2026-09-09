#include "Player.h"
#include <iostream>
#include <algorithm>

void Player::setBaseAttack(int value) { baseAttack = std::max(1, value); }
void Player::setHp(int value) { hp = std::max(0, std::min(maxHp, value)); }
void Player::addMaxHp(int amount) { maxHp += std::max(0, amount); }
void Player::setCriticalRate(int value) { critRate = std::clamp(value, 0, 100); }
void Player::setCriticalDamage(int value) { critDamage = std::max(0, value); }
void Player::addCR(int amount) { setCriticalRate(critRate + amount); }
void Player::addCD(int amount) { setCriticalDamage(critDamage + amount); }

Player::Player(std::string name)
    : name(name), maxHp(160), hp(160), shield(0), baseAttack(20), gold(20), critRate(10), critDamage(50) {}

std::string Player::getName() const {
    return name;
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

    std::cout << name << " 承受压力，剩余精神状态：" << hp << "/" << maxHp << "\n";
}

void Player::heal(int amount) {
    if (amount <= 0) return;
    hp = std::min(hp + amount, maxHp);
    std::cout << name << " 恢复了精神，当前状态：" << hp << "/" << maxHp << "\n";
}

void Player::addShield(int amount) {
    if (amount <= 0) return;
    shield += amount;
    std::cout << name << " 获得了 " << amount << " 点抗压，当前值：" << shield << "\n";
}

bool Player::isAlive() const {
    return hp > 0;
}

// ---------------- 攻击力 ----------------
int Player::getAttack() const {
    return baseAttack;
}

// ---------------- 金币 ----------------
int Player::getGold() const {
    return gold;
}

void Player::addGold(int amount) {
    if (amount <= 0) return;
    gold += amount;
    std::cout << name << " 获得了 " << amount << " 元生活费，当前拥有：" << gold << "元\n";
}

bool Player::spendGold(int amount) {
    if (amount > gold) {
        std::cout << "生活费不足！\n";
        return false;
    }
    gold -= amount;
    return true;
}

// ---------------- 背包----------------
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

// ---------------- 状态显示 ----------------
void Player::printStatus() const {
    std::cout << "========== 学业概况 ==========\n";
    std::cout << "姓名：" << name << "\n";
    std::cout << "精神状态：" << hp << "/" << maxHp << "  抗压：" << shield << "\n";
    std::cout << "学力：" << getAttack() << "\n";
    std::cout << "生活费：" << gold << "元\n";
    std::cout << "灵光（暴击率）：" << critRate << "%  超常发挥（暴击伤害）：" << critDamage << "%\n";
    std::cout << "背包物品数：" << bag.size() << "\n";
    std::cout << "==============================\n";
}

// 只恢复现有角色属性，攻击力不再来自装备或升级。
void Player::loadRawState(int hp_,int maxHp_,int shield_,int baseAttack_,int gold_,int cr,int cd) {
    hp=hp_; maxHp=maxHp_; shield=shield_; baseAttack=baseAttack_; gold=gold_;
    setCriticalRate(cr);
    setCriticalDamage(cd);
}
