#include "Game.h"
#include <iostream>
#include <limits>
Game::Game() {
    createMap();
}
void Game::createMap() {
    rooms_.emplace_back(1, "初始", "这里开始。请准备好。", RoomType::Start);
    rooms_.emplace_back(2, "第一次选择分支", "选择一条小路：战斗，或处理一个随机事件。", RoomType::Battle);
    rooms_.emplace_back(3, "小 Boss房间", "小 Boss 挡住了前往的道路。", RoomType::Boss);
    rooms_.emplace_back(4, "第二次选择分支", "选择拜访商店，或探索一处随机区域。", RoomType::Shop);
    rooms_.emplace_back(5, "休整", "这是挑战最终 Boss 前最后一次整备机会。", RoomType::Rest);
    rooms_.emplace_back(6, "最终 Boss", "最终 Boss 正在城堡深处等待。", RoomType::Boss);
}

void Game::start() {
    std::cout << "欢迎来到！\n";

    enterRoom(1);
    chooseBranch1();
    enterRoom(3);
    runBattle(true);

    chooseBranch2();
    enterRoom(5);
    runRest();

    enterRoom(6);
    runBattle(true);

    std::cout << "\n恭喜你击败最终 Boss，游戏通关！\n";
}

void Game::enterRoom(int id) {
    currentRoomId_ = id;
    rooms_.at(id - 1).showInfo();
}

void Game::chooseBranch1() {
    enterRoom(2);
    std::cout << "\n请选择小分支 1：\n";
    std::cout << "战斗\n";
    std::cout << "2:随机\n";

    if (readChoice(1, 2) == 1) {
        runBattle(false);
    } else {
        runEvent();
    }
}

void Game::chooseBranch2() {
    enterRoom(4);
    std::cout << "\n请选择小分支 2：\n";
    std::cout << "1. 进入商店\n";
    std::cout << "2. 随机\n";

    if (readChoice(1, 2) == 1) {
        runShop();
    } else {
        runEvent();
    }
}

int Game::readChoice(int min, int max) {
    int choice = 0;
    while (true) {
        std::cout << "请输入 " << min << " - " << max << "：";
        if (std::cin >> choice && choice >= min && choice <= max) {
            return choice;
        }
        std::cout << "输入无效，请重新输入。\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void Game::runBattle(bool isBoss) {
    // 之后由“敌人与战斗模块”组员替换为 BattleSystem::start(player, enemy)。
    std::cout << (isBoss ? "Boss 战" : "普通战斗") << "开始……（当前为测试占位逻辑）\n";
    std::cout << "战斗胜利，获得奖励！\n";
}

void Game::runEvent() {
    // 之后由“事件/物品模块”组员处理随机事件、加血、扣血或发放道具。
    std::cout << "事件触发：你发现一个补给箱，获得 10 金币！（测试占位逻辑）\n";
}

void Game::runShop() {
    // 之后由“物品/商店模块”组员处理商品、金币和武器升级。
    std::cout << "商店开启：你检查了商品并完成整备。（测试占位逻辑）\n";
}

void Game::runRest() {
    // 之后由“玩家模块”组员接入 Player::recover() 等函数。
    std::cout << "休整完成：恢复生命值，准备最终战。（测试占位逻辑）\n";
}