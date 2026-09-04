#pragma once
#include <string>
class Player;

class Room
{
public:
    std::string name;
    std::string description;
    Room(std::string n, std::string d);
    virtual void enter(Player* p) = 0; // 进入房间执行逻辑，纯虚函数
};

// 各个房间子类
class StartRoom : public Room
{
public:
    StartRoom();
    void enter(Player* p) override;
};

class BattleRoom : public Room
{
public:
    BattleRoom();
    void enter(Player* p) override;
};

class EventRoom : public Room
{
public:
    EventRoom();
    void enter(Player* p) override;
};

class RewardRoom : public Room
{
public:
    RewardRoom();
    void enter(Player* p) override;
};

class ShopRoom : public Room
{
public:
    ShopRoom();
    void enter(Player* p) override;
};

class RestRoom : public Room
{
public:
    RestRoom();
    void enter(Player* p) override;
};

class BossRoom : public Room
{
public:
    std::string bossName;
    int bossHp;
    int bossAttack;
    BossRoom(std::string n, std::string desc, std::string bn, int bhp, int batk);
    void enter(Player* p) override;
};
