#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "Item.h"  // 成员4的正式道具类；武器也是Item（type == "weapon"）

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

    // ---- 背包（消耗品和武器都放这里，Item是成员4的类）----
    void addItem(const Item& item);
    bool removeItem(const std::string& itemName);
    const std::vector<Item>& getBag() const;

    // ---- 武器（按名字从背包里装备，玩家同时只装备一把）----
    void equipWeapon(const std::string& weaponName); // 名字不存在或不是武器时保持原装备并提示
    const Item& getWeapon() const;                   // 没装备时返回默认武器"拳头"
    std::string getEquippedWeaponName() const;       // 没装备时返回空串，SaveManager存档用

    // ---- 状态显示 ----
    void printStatus() const;

    // ---- 读档专用：直接恢复原始数值，不走升级/回血等业务逻辑 ----
    void loadRawState(int level, int exp, int hp, int maxHp, int shield, int baseAttack, int gold);

private:
    std::string name;

    int level;
    int exp;
    int expToNextLevel;

    int maxHp;
    int hp;
    int shield;

    int baseAttack;
    Item equippedWeapon; // 当前装备武器的副本。不用指向背包元素的指针：vector扩容会使指针失效
    bool hasWeapon;      // 是否真的装备了武器（false时equippedWeapon是默认"拳头"，加成0）

    int gold;
    std::vector<Item> bag;

    void levelUp();
};

#endif
