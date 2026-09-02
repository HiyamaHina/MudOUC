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
    if (item.type == "potion") {
        player.heal(item.healAmount);
        std::cout << player.getName() << " 使用了 " << item.name
                  << "，恢复了 " << item.healAmount << " 点生命值。"
                  << " (当前HP: " << player.getHp() << "/" << player.getMaxHp() << ")" << std::endl;
        player.removeItem(item.name);
    } else if (item.type == "shield") {
        player.addShield(item.shieldAmount);
        std::cout << player.getName() << " 使用了 " << item.name
                  << "，获得了 " << item.shieldAmount << " 点护盾。"
                  << " (当前护盾: " << player.getShield() << ")" << std::endl;
        player.removeItem(item.name);
    } else if (item.type == "weapon") {
        player.equipWeapon(item.name);
        std::cout << player.getName() << " 装备了武器：" << item.name
                  << "（攻击加成 +" << item.attackBonus << "）" << std::endl;
        // 注意：武器不从背包里移除。Player.h 里 equippedWeapon 是指向背包里
        // 某个元素的指针，武器本来就应该继续留在背包 vector 里。
    } else {
        // "key" 或其他剧情类道具，先只打印提示，具体触发逻辑交给房间/事件模块处理
        std::cout << item.name << " 是剧情/钥匙类道具，暂时没有直接效果。" << std::endl;
    }
}
