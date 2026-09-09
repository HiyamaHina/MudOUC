#include "Item.h"
#include "Player.h"
#include <iostream>

Item::Item(std::string name_, std::string type_, std::string description_,
           int price_, int healAmount_, int shieldAmount_, int attackBonus_)
    : name(std::move(name_)), type(std::move(type_)), description(std::move(description_)),
      price(price_), healAmount(healAmount_), shieldAmount(shieldAmount_), attackBonus(attackBonus_) {}

void Item::use() const {
    std::cout << "[" << name << "] " << description << std::endl;
}

void applyItemEffect(Player& player, const Item& item) {
    if (item.type == "campus") {
        if (item.healAmount > 0) player.heal(item.healAmount);
        if (item.shieldAmount > 0) player.addShield(item.shieldAmount);
        if (item.attackBonus > 0) player.setBaseAttack(player.getBaseAttack() + item.attackBonus);
        player.removeItem(item.name);
        std::cout << "使用了 " << item.name << "：" << item.description << std::endl;
    } else if (item.type == "potion") {
        player.heal(item.healAmount);
        std::cout << player.getName() << " 使用了 " << item.name
                  << "，恢复了 " << item.healAmount << " 点精神状态。"
                  << " (当前HP: " << player.getHp() << "/" << player.getMaxHp() << ")" << std::endl;
        player.removeItem(item.name);
    } else if (item.type == "shield") {
        player.addShield(item.shieldAmount);
        std::cout << player.getName() << " 使用了 " << item.name
                  << "，获得了 " << item.shieldAmount << " 点抗压。"
                  << " (当前护盾: " << player.getShield() << ")" << std::endl;
        player.removeItem(item.name);
    } else {
        // "key" 或其他剧情类道具，先只打印提示，具体触发逻辑交给房间/事件模块处理
        std::cout << item.name << " 是剧情/钥匙类道具，暂时没有直接效果。" << std::endl;
    }
}
