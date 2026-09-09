#include "SaveManager.h"
#include "Item.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

namespace {
    // 按分隔符读取存档字段。
    std::vector<std::string> splitByBar(const std::string& line) {
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;
        while (std::getline(ss, field, '|')) {
            fields.push_back(field);
        }
        return fields;
    }
}

SaveManager::SaveManager(std::string filePath_) : filePath(std::move(filePath_)) {}

bool SaveManager::saveFileExists() const {
    std::ifstream ifs(filePath);
    return ifs.good();
}

void SaveManager::savePlayer(const Player& player) const {
    std::ofstream ofs(filePath);
    if (!ofs.is_open()) {
        std::cerr << "[SaveManager] 无法打开文件写入: " << filePath << std::endl;
        return;
    }

    ofs << "NAME|" << player.getName() << "\n";
    ofs << "HP|" << player.getHp() << "\n";
    ofs << "MAXHP|" << player.getMaxHp() << "\n";
    ofs << "SHIELD|" << player.getShield() << "\n";
    ofs << "BASEATTACK|" << player.getBaseAttack() << "\n";
    ofs << "GOLD|" << player.getGold() << "\n";
    ofs << "CR|" << player.getCR() << "\n";
    ofs << "CD|" << player.getCD() << "\n";

    for (const auto& it : player.getBag()) {
        ofs << "ITEM|" << it.name << "|" << it.type << "|" << it.description << "|"
            << it.price << "|" << it.healAmount << "|" << it.shieldAmount << "|" << it.attackBonus << "\n";
    }

    std::cout << "[SaveManager] 玩家数据已保存到 " << filePath << std::endl;
}

Player SaveManager::loadPlayer() const {
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        std::cout << "[SaveManager] 找不到存档文件 " << filePath << "，返回一个新玩家。" << std::endl;
        return Player("Hero");
    }

    std::string name = "Hero";
    int hp = 100, maxHp = 100, shield = 0, baseAttack = 10, gold = 0;
    int cr = 10, cd = 50; // 兼容没有暴击字段的校园存档
    std::vector<Item> items;

    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        auto fields = splitByBar(line);
        if (fields.empty()) continue;
        const std::string& tag = fields[0];

        if (tag == "NAME" && fields.size() >= 2) name = fields[1];
        else if (tag == "HP" && fields.size() >= 2) hp = std::stoi(fields[1]);
        else if (tag == "MAXHP" && fields.size() >= 2) maxHp = std::stoi(fields[1]);
        else if (tag == "SHIELD" && fields.size() >= 2) shield = std::stoi(fields[1]);
        else if (tag == "BASEATTACK" && fields.size() >= 2) baseAttack = std::stoi(fields[1]);
        else if (tag == "GOLD" && fields.size() >= 2) gold = std::stoi(fields[1]);
        else if (tag == "CR" && fields.size() >= 2) cr = std::stoi(fields[1]);
        else if (tag == "CD" && fields.size() >= 2) cd = std::stoi(fields[1]);
        else if (tag == "ITEM" && fields.size() >= 8) {
            Item it;
            it.name = fields[1];
            it.type = fields[2];
            it.description = fields[3];
            it.price = std::stoi(fields[4]);
            it.healAmount = std::stoi(fields[5]);
            it.shieldAmount = std::stoi(fields[6]);
            it.attackBonus = std::stoi(fields[7]);
            if (it.type == "potion" || it.type == "shield" || it.type == "campus" || it.type == "key") items.push_back(it);
        }
    }

    Player player(name);
    player.loadRawState(hp, maxHp, shield, baseAttack, gold, cr, cd);

    for (const auto& it : items) {
        player.addItem(it);
    }


    std::cout << "[SaveManager] 已从 " << filePath << " 读取存档。" << std::endl;
    return player;
}

void SaveManager::saveProgress(const GameProgress& progress) const {
    std::ofstream ofs(filePath + ".progress");
    if (!ofs.is_open()) {
        std::cerr << "[SaveManager] 无法打开进度文件写入" << std::endl;
        return;
    }
    ofs << "ROOM|" << progress.currentRoomName << "\n";
    ofs << "STAGE|" << progress.stageIndex << "\n";
    ofs << "VERSION|" << progress.formatVersion << "\n";
    ofs << "ROUTE|" << progress.route << "\n";
    ofs << "ENEMYHP|" << progress.enemyHp << "\n";
    ofs << "EVENT|" << progress.pendingEvent << "\n";
}

GameProgress SaveManager::loadProgress() const {
    GameProgress progress; // 默认值：StartRoom / 第0关
    std::ifstream ifs(filePath + ".progress");
    if (!ifs.is_open()) return progress;

    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        auto fields = splitByBar(line);
        if (fields.empty()) continue;
        if (fields[0] == "ROOM" && fields.size() >= 2) progress.currentRoomName = fields[1];
        else if (fields[0] == "STAGE" && fields.size() >= 2) progress.stageIndex = std::stoi(fields[1]);
        else if (fields[0] == "VERSION" && fields.size() >= 2) progress.formatVersion = std::stoi(fields[1]);
        else if (fields[0] == "ROUTE" && fields.size() >= 2) progress.route = std::stoi(fields[1]);
        else if (fields[0] == "ENEMYHP" && fields.size() >= 2) progress.enemyHp = std::stoi(fields[1]);
        else if (fields[0] == "EVENT" && fields.size() >= 2) progress.pendingEvent = std::stoi(fields[1]);
    }
    return progress;
}

void SaveManager::save(const Player& player, const GameProgress& progress) const {
    savePlayer(player);
    saveProgress(progress);
}

bool SaveManager::load(Player& outPlayer, GameProgress& outProgress) const {
    if (!saveFileExists()) return false;
    outPlayer = loadPlayer();
    outProgress = loadProgress();
    return true;
}
