#pragma once
#include <string>
class Player;

// potion：治疗；shield：护盾；campus：校园消耗品；key：剧情道具。
class Item {
public:
    Item() = default;
    Item(std::string name, std::string type, std::string description,
         int price=0, int healAmount=0, int shieldAmount=0, int attackBonus=0);
    std::string name;
    std::string type;
    std::string description;
    int price=0;
    int healAmount=0;
    int shieldAmount=0;
    int attackBonus=0; // 使用校园物品后永久增加的基础攻击力
    void use() const;
};
void applyItemEffect(Player& player, const Item& item);
