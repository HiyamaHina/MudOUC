
#ifndef MUDOUC_GAME_H
#define MUDOUC_GAME_H
#pragma once
#include "Room.h"

#include "../module5/CommandParser.h"
#include "../Player.h"
#include "../SaveManager.h"

#include <vector>

class Game {
public:
    Game();
    void start();

private:
    void createMap();//创建地图
    void enterRoom(int id);//进入房间
    void chooseBranch1();//选择上分支
    void chooseBranch2();//选择下分支
    int readChoice(int min, int max);
    void move();
    void useItem();
    void save();
    void load();
    void showRoomScreen() const;
    void showBag() const;
    void attack();

    // 以下四个函数是组员模块接入的位置。
    void runBattle(bool isBoss);
    void runEvent();
    void runShop();
    void runRest();

    Player player_{"学海行者"};
    SaveManager saves_{"save.txt"};
    // 简单战斗接入，正式战斗模块就绪后可替换 attack/runBattle。
    std::string enemyName_;
    int enemyHp_ = 0;
    int enemyMaxHp_ = 0;
    int enemyAttack_ = 0;
    int enemyReward_ = 0;
    int route_ = 0;
    int pendingEvent_ = 0;

    std::vector<Room> rooms_;
    int currentRoomId_ = 0;
};
#endif //MUDOUC_GAME_H
