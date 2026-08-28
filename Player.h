#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

// -----------------------------------------------------------------
// 注意：Item和Weapon的完整定义由成员4（物品/存档模块）负责设计。
// 下面是占位版本，字段按组长发的架构表来的：
//   Item  —— 消耗品（药水/强化道具），有 name / effect / price
//   Weapon —— 武器，有 name / attackBonus / level / upgradePrice
// 等成员4定稿Item.h和Weapon.h之后，删掉下面这两段，改成：
//   #include "Item.h"
//   #include "Weapon.h"
// 只要字段名对得上，Player.cpp基本不用改。
// -----------------------------------------------------------------
struct Item {
    std::string name;
    std::string effect; // 效果描述，比如"heal"/"shield"，具体怎么解析由成员4定
    int price = 0;
};

struct Weapon {
    std::string name = "拳头";
    int attackBonus = 0;
    int level = 1;
    int upgradePrice = 0;
};

class Player {
public:
    Player(std::string name);

    // ---- 基本信息 ----
    std::string getName() const;

    // ---- 等级与经验 ----
    int getLevel() const;
    int getExp() const;
    void gainExp(int amount);

    // ---- 生命与护盾 ----
    int getHp() const;
    int getMaxHp() const;
    int getShield() const;
    void takeDamage(int amount);
    void heal(int amount);
    void addShield(int amount);
    bool isAlive() const;

    // ---- 攻击力 ----
    int getAttack() const; // 基础攻击力 + 当前武器加成

    // ---- 金币 ----
    int getGold() const;
    void addGold(int amount);
    bool spendGold(int amount);

    // ---- 背包（只装消耗品Item，武器不放这里）----
    void addItem(const Item& item);
    bool removeItem(const std::string& itemName);
    const std::vector<Item>& getBag() const;

    // ---- 武器（玩家同时只装备一把武器）----
    void equipWeapon(const Weapon& weapon); // 直接替换当前武器
    const Weapon& getWeapon() const;

    // ---- 状态显示 ----
    void printStatus() const;

private:
    std::string name;

    int level;
    int exp;
    int expToNextLevel;

    int maxHp;
    int hp;
    int shield;

    int baseAttack;
    Weapon equippedWeapon; // 默认是"拳头"，没有攻击加成

    int gold;
    std::vector<Item> bag;

    void levelUp();
};

#endif
