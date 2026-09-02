#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <string>
#include "Player.h"

// 游戏整体进度信息（当前房间、阶段等）。
// Room / Game 类还没定稿，先用一个轻量结构占位，等成员1的 Room/Game 写好后，
// 再把 currentRoomName 换成真正从 Game 里取值即可，SaveManager 对外的用法基本不用变。
struct GameProgress {
    std::string currentRoomName = "StartRoom"; // 当前所在房间的名字/编号
    int stageIndex = 0;                        // 当前处于第几关/第几阶段，方便以后扩展
};

// 存档管理类。对应 UML 图里的 SaveManager。
// 负责把 Player 的状态（等级、经验、血量、护盾、金币、背包、装备的武器）
// 以及游戏进度写入文本文件；也负责从文件读回来，恢复出一个 Player 对象。
class SaveManager {
public:
    explicit SaveManager(std::string filePath = "save.txt");

    // ---- 玩家存档 ----
    void savePlayer(const Player& player) const;
    Player loadPlayer() const;   // 读档失败（文件不存在/格式不对）时返回一个默认新玩家 "Hero"

    // ---- 游戏进度存档（当前房间/阶段） ----
    void saveProgress(const GameProgress& progress) const;
    GameProgress loadProgress() const;

    // ---- 一次性把玩家+进度都存/读了（最常用的入口，CommandParser模块可以直接调这两个） ----
    void save(const Player& player, const GameProgress& progress) const;
    bool load(Player& outPlayer, GameProgress& outProgress) const; // 返回是否读档成功

    bool saveFileExists() const;

private:
    std::string filePath;
};

#endif
