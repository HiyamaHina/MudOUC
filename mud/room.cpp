#include "Room.h"
#include "Player.h"
#include "EnemyEvent.h"
#include "ItemWeapon.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Room::Room(std::string n, std::string d)
    : name(n), description(d)
{}

// ===================== 开始房间 StartRoom =====================
StartRoom::StartRoom()
    : Room("开始房间", "你开启了在每羊小学的一天！又是新一轮巴巴博弈，你在忽悠上发现于老师人帅心善，于是决定攒够100选课币allin于老师的课！")
{}

void StartRoom::enter(Player* p)
{
    std::cout << "\n===== " << name << " =====" << std::endl;
    std::cout << description << std::endl;
    std::cout << "请选择行动：" << std::endl;
    std::cout << "1.前去战斗，获取选课币" << std::endl;
    std::cout << "2.四处打探，触发随机事件" << std::endl;
    int sel;
    std::cin >> sel;
    if (sel == 1)
    {
        BattleRoom br;
        br.enter(p);
    }
    else
    {
        EventRoom er;
        er.enter(p);
    }
}

// ===================== 战斗房间 BattleRoom =====================
BattleRoom::BattleRoom()
    : Room("战斗房间", "你遇到了卡点投币的学长！")
{}

void BattleRoom::enter(Player* p)
{
    std::cout << "\n===== " << name << " =====" << std::endl;
    srand((unsigned)time(nullptr));
    Enemy e("学长", 40, 12);
    std::cout << "敌人：" << e.name << " 血量：" << e.hp << std::endl;

    while (e.hp > 0 && p->getHp() > 0)
    {
        std::cout << "\n请输入1攻击敌人：";
        int op;
        std::cin >> op;
        if(op == 1)
        {
            int dmg = 15 + rand() % 10;
            e.hp -= dmg;
            std::cout << "你造成" << dmg << "伤害！敌人剩余血量：" << e.hp << std::endl;
            if(e.hp <= 0) break;
            e.doAttack(p);
        }
    }

    if(p->getHp() <= 0)
    {
        std::cout << "你战败了，游戏结束！" << std::endl;
        return;
    }
    // 胜利奖励选课币
    int reward = 10 + rand() % 11;
    p->addCoin(reward);
    std::cout << "战斗胜利！获得" << reward << "选课币！" << std::endl;

    // 打完普通战斗，前往小Boss
    BossRoom smallBoss("小Boss房间", "教秘拦住了你！", "教秘", 70, 18);
    smallBoss.enter(p);
}

// ===================== 事件房间 EventRoom =====================
EventRoom::EventRoom()
    : Room("事件房间", "你遇到随机校园事件")
{}

void EventRoom::enter(Player* p)
{
    std::cout << "\n===== " << name << " =====" << std::endl;
    srand((unsigned)time(nullptr));
    int r = rand() % 3;
    if(r == 0)
    {
        std::cout << "帮老师整理课件，得到24选课币！" << std::endl;
        p->addCoin(24);
    }
    else if(r == 1)
    {
        std::cout << "捡到选课凭证，得到20选课币！" << std::endl;
        p->addCoin(20);
    }
    else
    {
        std::cout << "被崩溃的校园网坑走7选课币！" << std::endl;
        p->subCoin(7);
    }
    // 事件结束，进入小Boss
    BossRoom smallBoss("小Boss房间", "重修的学长拦住了你！", "学长",70,18);
    smallBoss.enter(p);
}

// ===================== 奖励房间 RewardRoom =====================
RewardRoom::RewardRoom()
    : Room("奖励房间", "你发现了隐藏的刷题点！")
{}

void RewardRoom::enter(Player* p)
{
    std::cout << "\n===== " << name << " =====" << std::endl;
    std::cout << "完成刷题，获得50选课币！" << std::endl;
    p->addCoin(50);
}

// ===================== 商店房间 ShopRoom =====================
ShopRoom::ShopRoom()
    : Room("选课黑市商店", "你来到选课黑市，可以消耗选课币购买道具")
{}

void ShopRoom::enter(Player* p)
{
    std::cout << "\n===== " << name << " =====" << std::endl;
    std::cout << "当前选课币：" << p->getCoin() << std::endl;
    std::cout << "1.选课加速卡（消耗5币，战斗奖励+5）" << std::endl;
    std::cout << "2.护身符（消耗10币，抵挡一次扣币）" << std::endl;
    std::cout << "3.离开商店" << std::endl;
    int sel;
    std::cin >> sel;
    if(sel == 1)
    {
        if(p->getCoin() >= 5)
        {
            p->subCoin(5);
            std::cout << "购买选课加速卡成功！" << std::endl;
        }
        else std::cout << "选课币不足！" << std::endl;
    }
    else if(sel == 2)
    {
        if(p->getCoin() >=10)
        {
            p->subCoin(10);
            std::cout << "购买护身符成功！" << std::endl;
        }
        else std::cout << "选课币不足！" << std::endl;
    }
}

// ===================== 休整房间 RestRoom =====================
RestRoom::RestRoom()
    : Room("休整房间", "旅途疲惫，你在此休整")
{}

void RestRoom::enter(Player* p)
{
    std::cout << "\n===== " << name << " =====" << std::endl;
    std::cout << "你休息恢复血量，血量回满！" << std::endl;
    p->setHp(100);
    p->showStatus();
    std::cout << "休整完毕，前往最终Boss！" << std::endl;
    // 休整完，进入最终Boss
    BossRoom finalBoss("最终Boss房间", "助教挡在于老师课堂门前！", "助教",100,22);
    finalBoss.enter(p);
}

// ===================== BossRoom 小Boss /最终Boss共用 =====================
BossRoom::BossRoom(std::string n, std::string desc, std::string bn, int bhp, int batk)
    : Room(n, desc), bossName(bn), bossHp(bhp), bossAttack(batk)
{}

void BossRoom::enter(Player* p)
{
    std::cout << "\n===== " << name << " =====" << std::endl;
    std::cout << description << std::endl;
    Enemy e(bossName, bossHp, bossAttack);
    std::cout << "BOSS：" << e.name << " 血量：" << e.hp << std::endl;

    while(e.hp >0 && p->getHp()>0)
    {
        std::cout << "\n输入1攻击BOSS：";
        int op;
        std::cin >> op;
        if(op == 1)
        {
            int dmg = 15 + rand()%10;
            e.hp -= dmg;
            std::cout << "你造成" << dmg << "伤害！BOSS剩余：" << e.hp << std::endl;
            if(e.hp <= 0) break;
            e.doAttack(p);
        }
    }

    if(p->getHp() <= 0)
    {
        std::cout << "你被BOSS击败，游戏结束！" << std::endl;
        return;
    }

    // 判断：如果是小Boss，给奖励；如果是最终Boss，直接结算结局
    if(bossName == "教务处干事")
    {
        std::cout << "击败小Boss！获得25选课币！" << std::endl;
        p->addCoin(25);

        // 小Boss打完，二选一：商店/奇遇事件
        std::cout << "\n打完小Boss，请选择下一步：" << std::endl;
        std::cout << "1.进入选课币黑市" << std::endl;
        std::cout << "2.继续冒险（事件/奖励）" << std::endl;
        int sel;
        std::cin >> sel;
        if(sel == 1)
        {
            ShopRoom sr;
            sr.enter(p);
        }
        else
        {
            EventRoom er;
            er.enter(p);
            RewardRoom rr;
            rr.enter(p);
        }
        // 走完之后进入休整房间
        RestRoom rest;
        rest.enter(p);
    }
    else // 最终Boss
    {
        std::cout << "\n===== 战斗结束，结算选课币！ =====" << std::endl;
        int coin = p->getCoin();
        if(coin >= 100)
        {
            std::cout << "恭喜！你的选课币" << coin << "，成功选上于老师的课！通关！" << std::endl;
        }
        else
        {
            std::cout << "很遗憾，选课币只有" << coin << "，不足100，落选！游戏结束。" << std::endl;
        }
    }
}
