#ifndef ITEM_H
#define ITEM_H

#include <string>

// 前向声明，不直接 #include "Player.h"
// 原因：Player.h 里会 #include "Item.h"（把成员2写的占位 struct Item 换成这个真正的 Item 类），
// 如果 Item.h 也去 include Player.h，就会造成两个头文件互相 include（循环依赖），编译会出问题。
// 所以这里只做前向声明，真正用到 Player 的地方放到 Item.cpp 里去写。
class Player;

// 道具类。对应组内 UML 图里的 Item 类，同时兼顾成员2 Player.h 里对 Item 的要求
// （成员2的占位结构里要求 Item 至少要有 name 和 attackBonus 这两个字段，这里全部保留，
//   而且都是 public 的，Player 模块可以像原来一样直接用 item.name / item.attackBonus 访问）。
//
// type 字段的约定取值（字符串，方便存档直接写文件、也方便以后扩展新类型）：
//   "potion" 治疗药水 | "shield" 护盾药水 | "weapon" 武器/攻击强化道具 | "key" 钥匙/剧情道具
class Item {
public:
    Item() = default;
    Item(std::string name, std::string type, std::string description,
         int price = 0, int healAmount = 0, int shieldAmount = 0, int attackBonus = 0);

    // ---- 属性：全部 public，保持和成员2占位结构一样的用法 ----
    std::string name;
    std::string type;
    std::string description;
    int price = 0;         // 商店价格，成员1/5的商店模块会用到
    int healAmount = 0;    // 恢复生命值，type == "potion" 时有效
    int shieldAmount = 0;  // 护盾值，type == "shield" 时有效
    int attackBonus = 0;   // 攻击加成，type == "weapon" 时有效
                            // ★ 这个字段名必须叫 attackBonus，Player.h 的 equipWeapon 逻辑依赖它

    // 打印这个道具的说明文字（不会直接改动 Player 数据，避免循环依赖）
    void use() const;
};

// 把道具的效果真正作用到玩家身上：加血、加护盾、装备武器等。
// 声明放在 Item.h（因为逻辑上属于"道具"要做的事），但实现在 Item.cpp 里，
// 那边可以放心 #include "Player.h"，因为 Player.h 不会反过来 include 这个函数声明所在的实现文件。
void applyItemEffect(Player& player, const Item& item);

#endif
