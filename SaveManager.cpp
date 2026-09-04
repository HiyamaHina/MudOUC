#include "SaveManager.h"
#include "Item.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

namespace {
    // 把一行按 '|' 分割成若干段，比如 "ITEM|铁剑|weapon|一把普通的铁剑|50|0|0|5"
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

    // Player.h 只提供 getAttack()（基础攻击+武器加成后的总值），没有单独的“基础攻击力”getter。
    // 这里用 “总攻击 - 当前装备武器的attackBonus” 反推出基础攻击力，这样就不需要
    // Player 类额外暴露 baseAttack 字段了。
    std::string weaponName = player.getEquippedWeaponName();
    int weaponBonus = 0;
    for (const auto& it : player.getBag()) {
        if (it.name == weaponName) {
            weaponBonus = it.attackBonus;
            break;
        }
    }
    int baseAttack = player.getAttack() - weaponBonus;

    ofs << "NAME|" << player.getName() << "\n";
    ofs << "LEVEL|" << player.getLevel() << "\n";
    ofs << "EXP|" << player.getExp() << "\n";
    ofs << "HP|" << player.getHp() << "\n";
    ofs << "MAXHP|" << player.getMaxHp() << "\n";
    ofs << "SHIELD|" << player.getShield() << "\n";
    ofs << "BASEATTACK|" << baseAttack << "\n";
    ofs << "GOLD|" << player.getGold() << "\n";
    ofs << "WEAPON|" << (weaponName.empty() ? "NONE" : weaponName) << "\n";

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
    int level = 1, exp = 0, hp = 100, maxHp = 100, shield = 0, baseAttack = 10, gold = 0;
    std::string equippedWeaponName;
    std::vector<Item> items;

    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        auto fields = splitByBar(line);
        if (fields.empty()) continue;
        const std::string& tag = fields[0];

        if (tag == "NAME" && fields.size() >= 2) name = fields[1];
        else if (tag == "LEVEL" && fields.size() >= 2) level = std::stoi(fields[1]);
        else if (tag == "EXP" && fields.size() >= 2) exp = std::stoi(fields[1]);
        else if (tag == "HP" && fields.size() >= 2) hp = std::stoi(fields[1]);
        else if (tag == "MAXHP" && fields.size() >= 2) maxHp = std::stoi(fields[1]);
        else if (tag == "SHIELD" && fields.size() >= 2) shield = std::stoi(fields[1]);
        else if (tag == "BASEATTACK" && fields.size() >= 2) baseAttack = std::stoi(fields[1]);
        else if (tag == "GOLD" && fields.size() >= 2) gold = std::stoi(fields[1]);
        else if (tag == "WEAPON" && fields.size() >= 2) equippedWeaponName = fields[1];
        else if (tag == "ITEM" && fields.size() >= 8) {
            Item it;
            it.name = fields[1];
            it.type = fields[2];
            it.description = fields[3];
            it.price = std::stoi(fields[4]);
            it.healAmount = std::stoi(fields[5]);
            it.shieldAmount = std::stoi(fields[6]);
            it.attackBonus = std::stoi(fields[7]);
            items.push_back(it);
        }
    }

    Player player(name);
    // ★ loadRawState 目前 Player.h 里还没有，需要成员2补一个这样的方法，
    //   否则读档时没法把等级/经验/血量精确地还原回去。详见交付说明里的“需要成员2配合”部分。
    player.loadRawState(level, exp, hp, maxHp, shield, baseAttack, gold);

    for (const auto& it : items) {
        player.addItem(it);
    }
    if (!equippedWeaponName.empty() && equippedWeaponName != "NONE") {
        player.equipWeapon(equippedWeaponName);
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
