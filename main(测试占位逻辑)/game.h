
#ifndef MUDOUC_GAME_H
#define MUDOUC_GAME_H
#pragma once
#include "Room.h"

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

    // 以下四个函数是组员模块接入的位置。
    void runBattle(bool isBoss);
    void runEvent();
    void runShop();
    void runRest();

    std::vector<Room> rooms_;
    int currentRoomId_ = 0;
};
#endif //MUDOUC_GAME_H