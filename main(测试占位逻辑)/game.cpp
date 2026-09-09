#include "game.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

Game::Game() {
    createMap();
    // 开局补给，便于第一次战斗时使用物品。
    player_.addItem(Item("治疗药水", "potion", "恢复30点生命", 10, 30));
    player_.addItem(Item("护盾药水", "shield", "增加15点护盾", 10, 0, 15));
}

void Game::createMap() {
    rooms_.emplace_back(1, "起点营地", "准备补给，输入4选择第一条路线。", RoomType::Start);
    rooms_.emplace_back(2, "林间岔路", "这里可能遭遇敌人，也可能发现补给。", RoomType::Battle);
    rooms_.emplace_back(3, "守卫关口", "击败小Boss才能继续前进。", RoomType::Boss);
    rooms_.emplace_back(4, "旅人驿站", "可以前往商店，或者探索补给区域。", RoomType::Shop);
    rooms_.emplace_back(5, "休整营地", "在这里恢复生命，准备最终挑战。", RoomType::Rest);
    rooms_.emplace_back(6, "最终Boss大厅", "击败最终Boss即可通关。", RoomType::Boss);
}

void Game::start() {
    using namespace module5;
    ConsoleUI ui(std::cin, std::cout, prepareConsole());
    enterRoom(1);
    ui.setHeader([this] { showRoomScreen(); });
    ui.bind(Action::Attack, [this] { attack(); });
    ui.bind(Action::Bag, [this] { showBag(); });
    ui.bind(Action::Use, [this] { useItem(); });
    ui.bind(Action::Move, [this] { move(); });
    ui.bind(Action::Save, [this] { save(); });
    ui.bind(Action::Load, [this] { load(); });
    ui.bind(Action::Look, [this] { player_.printStatus(); });
    ui.run();
}

// 界面只读取数据，不在显示时发奖励或修改血量。
void Game::showRoomScreen() const {
    const Room& room = rooms_.at(currentRoomId_ - 1);
    std::cout << "\n============================================================\n"
              << "  货币战争  |  " << currentRoomId_ << " / 6  " << room.getName() << '\n'
              << "------------------------------------------------------------\n"
              << "  " << player_.getName() << "  Lv." << player_.getLevel()
              << "  生命 " << player_.getHp() << '/' << player_.getMaxHp()
              << "  护盾 " << player_.getShield() << "  金币 " << player_.getGold() << '\n'
              << "  攻击 " << player_.getAttack() << "  武器 " << player_.getWeapon().name
              << "  背包 " << player_.getBag().size() << " 件\n"
              << "------------------------------------------------------------\n"
              << "  " << room.getDescription() << '\n';
    if (!player_.isAlive())
        std::cout << "  状态：战败。可以输入6读档，或0退出。\n";
    else if (enemyHp_ > 0)
        std::cout << "  敌人：" << enemyName_ << "  生命 " << enemyHp_ << '/' << enemyMaxHp_
                  << "  攻击 " << enemyAttack_ << "\n  状态：战斗中，输入1攻击，击败敌人后才能离开。\n";
    else if (currentRoomId_ == 6)
        std::cout << "  状态：通关！最终Boss已被击败。\n";
    else
        std::cout << "  状态：安全，可以整理背包，输入4继续前进。\n";
}

void Game::showBag() const {
    std::cout << "\n[背包]\n";
    const auto& bag = player_.getBag();
    if (bag.empty()) { std::cout << "背包为空。\n"; return; }
    for (std::size_t i = 0; i < bag.size(); ++i)
        std::cout << i + 1 << ". " << bag[i].name << "：" << bag[i].description << '\n';
}

void Game::move() {
    if (!player_.isAlive()) { std::cout << "你已战败，请读档或退出。\n"; return; }
    if (enemyHp_ > 0) { std::cout << "敌人挡住了出口，请先按1攻击。\n"; return; }
    switch (currentRoomId_) {
    case 1: chooseBranch1(); break;
    case 2: enterRoom(3); runBattle(true); break;
    case 3: chooseBranch2(); break;
    case 4: enterRoom(5); runRest(); break;
    case 5: enterRoom(6); runBattle(true); break;
    default: std::cout << "已经通关，可以保存或退出。\n"; break;
    }
}

void Game::useItem() {
    if (!player_.isAlive()) { std::cout << "战败后不能使用物品，请读档或退出。\n"; return; }
    const auto& bag = player_.getBag();
    showBag();
    if (bag.empty()) return;
    std::cout << "选择物品编号，0取消。本版本使用物品不消耗攻击回合。\n";
    const int choice = readChoice(0, static_cast<int>(bag.size()));
    if (choice == 0) return;
    const Item item = bag.at(choice - 1); // 使用时会删除背包元素，因此先复制。
    applyItemEffect(player_, item);
}

// 一次按键就是一个回合：玩家攻击 -> 敌人存活才反击。
void Game::attack() {
    if (!player_.isAlive()) { std::cout << "你已战败，请读档或退出。\n"; return; }
    if (enemyHp_ <= 0) { std::cout << "当前没有敌人，无需攻击。按4前往下一房间。\n"; return; }
    const int damage = player_.getAttack();
    enemyHp_ = std::max(0, enemyHp_ - damage);
    std::cout << "你对" << enemyName_ << "造成 " << damage << " 点伤害。\n";
    if (enemyHp_ == 0) {
        std::cout << "战斗胜利！获得 " << enemyReward_ << " 金币。\n";
        player_.addGold(enemyReward_);
        player_.gainExp(20);
        if (currentRoomId_ == 6) std::cout << "恭喜通关！\n";
        return; // 已击败的敌人不反击，再按攻击也不会重复发奖励。
    }
    std::cout << enemyName_ << "反击，攻击力 " << enemyAttack_ << "。\n";
    player_.takeDamage(enemyAttack_);
    if (!player_.isAlive()) std::cout << "你已战败，游戏结束。可以读取之前的存档。\n";
}

void Game::save() {
    GameProgress progress;
    progress.currentRoomName = std::to_string(currentRoomId_);
    progress.stageIndex = currentRoomId_ - 1;
    progress.formatVersion = 2;
    progress.route = route_;
    progress.enemyHp = enemyHp_;
    saves_.save(player_, progress);
}

void Game::load() {
    Player loaded = player_;
    GameProgress progress;
    if (!saves_.load(loaded, progress)) {
        std::cout << "没有找到存档，请先保存。\n";
        return;
    }
    if (progress.stageIndex < 0 || progress.stageIndex > 5)
        throw std::runtime_error("存档房间编号无效。");
    const int id = progress.stageIndex + 1;
    const bool branch = id == 2 || id == 4;
    if (progress.formatVersion != 2 || id < 1 || id > 6 ||
        progress.currentRoomName != std::to_string(id) ||
        (branch ? (progress.route != 1 && progress.route != 2) : progress.route != 0))
        throw std::runtime_error("存档版本或房间进度不兼容，请使用本版存档。");
    const int maxHp = id == 6 ? 80 : id == 3 ? 50 : (id == 2 && progress.route == 1) ? 30 : 0;
    if (progress.enemyHp < 0 || progress.enemyHp > maxHp)
        throw std::runtime_error("存档中的敌人血量无效。");
    // 读取不再触发事件、休息或战斗奖励。
    player_ = loaded;
    enterRoom(id);
    route_ = progress.route;
    if (maxHp > 0) runBattle(id != 2);
    enemyHp_ = progress.enemyHp;
    std::cout << "已恢复玩家、房间和敌人血量。\n";
}

void Game::enterRoom(int id) {
    currentRoomId_ = id;
    route_ = 0;
    enemyName_.clear();
    enemyHp_ = enemyMaxHp_ = enemyAttack_ = enemyReward_ = 0;
}

void Game::chooseBranch1() {
    std::cout << "\n[选择路线] 1 战斗小路   2 补给小路   0 取消\n";
    const int choice = readChoice(0, 2);
    if (choice == 0) return;
    enterRoom(2);
    route_ = choice;
    if (choice == 1) runBattle(false);
    else runEvent();
}

void Game::chooseBranch2() {
    std::cout << "\n[选择路线] 1 商店   2 补给区域   0 取消\n";
    const int choice = readChoice(0, 2);
    if (choice == 0) return;
    // 商店操作完成后进入房间，输入结束时不会留下未完成的进度。
    if (choice == 1) runShop();
    enterRoom(4);
    route_ = choice;
    if (choice == 2) runEvent();
}

int Game::readChoice(int min, int max) {
    return module5::CommandParser::readChoice(std::cin, std::cout, min, max);
}

// 基础敌人数值集中在这里。队友提供正式Enemy后，可替换此函数和attack。
void Game::runBattle(bool isBoss) {
    const bool finalBoss = currentRoomId_ == 6;
    enemyName_ = finalBoss ? "城堡领主" : isBoss ? "关口守卫" : "林地盗贼";
    enemyMaxHp_ = finalBoss ? 80 : isBoss ? 50 : 30;
    enemyHp_ = enemyMaxHp_;
    enemyAttack_ = finalBoss ? 10 : isBoss ? 8 : 5;
    enemyReward_ = finalBoss ? 50 : isBoss ? 25 : 10;
}

void Game::runEvent() {
    std::cout << "发现补给箱，获得10金币和一瓶治疗药水。\n";
    player_.addGold(10);
    player_.addItem(Item("治疗药水", "potion", "恢复30点生命", 10, 30));
}

void Game::runShop() {
    std::cout << "\n[商店] 金币：" << player_.getGold()
              << "\n1 治疗药水（10金币）  2 护盾药水（10金币）  0 离开\n";
    const int choice = readChoice(0, 2);
    if (choice == 0) return;
    if (!player_.spendGold(10)) { std::cout << "金币不足，没有购买。\n"; return; }
    player_.addItem(choice == 1 ? Item("治疗药水", "potion", "恢复30点生命", 10, 30)
                               : Item("护盾药水", "shield", "增加15点护盾", 10, 0, 15));
}

void Game::runRest() {
    player_.heal(player_.getMaxHp());
    std::cout << "休整完成，生命已恢复。输入4挑战最终Boss。\n";
}
