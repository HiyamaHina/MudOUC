#pragma once
#include <vector>
#include <string>
class Weapon;
class Item;

class Player
{
private:
    int hp;
    int coin;       // 选课币，游戏核心
    std::vector<Weapon*> weapons;
    std::vector<Item*> items;
public:
    Player();
    void addWeapon(Weapon* w);
    void addItem(Item* it);
    void removeItem(Item* it);
    void showStatus();

    int getHp() const { return hp; }
    void setHp(int h) { hp = h; }
    int getCoin() const { return coin; }
    void setCoin(int c) { coin = c; }
    void addCoin(int num) { coin += num; }
    void subCoin(int num) { coin -= num; }

    std::vector<Weapon*>& getWeapons() { return weapons; }
    std::vector<Item*>& getItems() { return items; }
};
