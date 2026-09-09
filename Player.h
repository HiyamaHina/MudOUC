#pragma once
#include <string>
#include <vector>
#include "Item.h"

class Player {
public:
    explicit Player(std::string name);
    std::string getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getShield() const;
    void takeDamage(int amount);
    void heal(int amount);
    void addShield(int amount);
    bool isAlive() const;
    int getAttack() const;
    int getCR() const { return critRate; }
    int getCD() const { return critDamage; }
    void setCriticalRate(int value);
    void setCriticalDamage(int value);
    void addCR(int amount);
    void addCD(int amount);
    int getBaseAttack() const { return baseAttack; }
    void setBaseAttack(int value);
    void setHp(int value);
    void addMaxHp(int amount);
    int getGold() const;
    void addGold(int amount);
    bool spendGold(int amount);
    void addItem(const Item& item);
    bool removeItem(const std::string& itemName);
    const std::vector<Item>& getBag() const;
    void printStatus() const;
    void loadRawState(int hp,int maxHp,int shield,int baseAttack,int gold,int cr=10,int cd=50);
private:
    std::string name;
    int maxHp;
    int hp;
    int shield;
    int baseAttack;
    int gold;
    int critRate;
    int critDamage; // 额外伤害百分比，50代表暴击造成150%的伤害
    std::vector<Item> bag;
};
