#include "game.h"
#include "Story.h"
#include "CampusContent.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

Game::Game() {
    createMap();
}

void Game::createMap() {
    rooms_.emplace_back(1,"学业初航","书卷铺陈，少年初渡学海。",RoomType::Start);
    rooms_.emplace_back(2,"初涉学海","大学生活，始于今日。",RoomType::Battle);
    rooms_.emplace_back(3,"课业初临","第一份作业来了。",RoomType::Battle);
    rooms_.emplace_back(4,"猝然一试","老师说：拿出纸笔。",RoomType::Boss);
    rooms_.emplace_back(5,"「机缘」","命途偶有岔路，福祸皆在其中。",RoomType::Shop);
    rooms_.emplace_back(6,"同舟共济","队友，永远是未知数。",RoomType::Battle);
    rooms_.emplace_back(7,"半程问道","期中已至，成败一试。",RoomType::Boss);
    rooms_.emplace_back(8,"「机缘」","命途偶有岔路，福祸皆在其中。",RoomType::Shop);
    rooms_.emplace_back(9,"泛舟文海","论文之海，浩瀚无边。",RoomType::Battle);
    rooms_.emplace_back(10,"偷得浮生","浮生半日闲，且将烦忧暂忘。",RoomType::Rest);
    rooms_.emplace_back(11,"终局渡劫","最后的大考，避无可避。",RoomType::Boss);
}
void Game::start() {
    using namespace module5;
    ConsoleUI ui(std::cin, std::cout, prepareConsole());
    std::cout << "\n================ 前情提要 ================\n" << storyText
              << "\n按回车开始游戏……" << std::flush;
    std::string ready;
    if (!std::getline(std::cin, ready)) return;
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
              << "  学海求生  |  " << currentRoomId_ << " / 11  " << room.getName() << '\n'
              << "------------------------------------------------------------\n"
              << "  " << player_.getName()
              << "  精神状态 " << player_.getHp() << '/' << player_.getMaxHp()
              << "  抗压 " << player_.getShield() << "  生活费 " << player_.getGold() << '\n'
              << "  学力 " << player_.getAttack()
              << "  背包 " << player_.getBag().size() << " 件\n"
              << "  灵光（暴击率） " << player_.getCR() << "%  超常发挥（暴击伤害） " << player_.getCD() << "%\n"
              << "------------------------------------------------------------\n"
              << "  " << room.getDescription() << '\n';
    if (!player_.isAlive())
        std::cout << "  状态：战败。可以输入6读档，或0退出。\n";
    else if (pendingEvent_ > 0) std::cout << "  状态：奇遇等待选择，输入4继续处理。\n";
    else if (enemyHp_ > 0)
        std::cout << "  任务：" << enemyName_ << "  剩余进度 " << enemyHp_ << '/' << enemyMaxHp_
                  << "  压力 " << enemyAttack_ << "\n  状态：战斗中，输入1继续完成任务，完成任务后才能离开。\n";
    else if (currentRoomId_ == 11)
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
    if (pendingEvent_ > 0) { runEvent(); return; }
    if (enemyHp_ > 0) { std::cout << "任务挡住了你的脚步，请先按1攻击。\n"; return; }
    if (currentRoomId_ == 11) { std::cout << "已经通关，可以保存或退出。\n"; return; }
    const int next = currentRoomId_ + 1;
    if (campus::battleBranch(next)) { chooseBranch1(); return; }
    if (campus::shopBranch(next)) { chooseBranch2(); return; }
    enterRoom(next);
    if (next == 10) runRest(); else runBattle(true);
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
    const bool critical = std::uniform_int_distribution<int>(1,100)(campus::random()) <= player_.getCR();
    const int damage = critical ? static_cast<int>(player_.getAttack() * (1.0 + player_.getCD()/100.0)) : player_.getAttack();
    if (critical) std::cout << "灵光乍现！暴击！\n";
    enemyHp_ = std::max(0, enemyHp_ - damage);
    std::cout << "你对" << enemyName_ << "完成 " << damage << " 点进度。\n";
    if (enemyHp_ == 0) {
        std::cout << "战斗胜利！获得 " << enemyReward_ << " 元。\n";
        player_.addGold(enemyReward_);
        if (currentRoomId_ == 11) std::cout << "恭喜通关！\n";
        return; // 已击败的敌人不反击，再按攻击也不会重复发奖励。
    }
    std::cout << enemyName_ << "反击，造成 " << enemyAttack_ << "点压力。\n";
    player_.takeDamage(enemyAttack_);
    if (!player_.isAlive()) std::cout << "你已战败，游戏结束。可以读取之前的存档。\n";
}

void Game::save() {
    GameProgress progress;
    progress.currentRoomName = std::to_string(currentRoomId_);
    progress.stageIndex = currentRoomId_ - 1;
    progress.formatVersion = 3;
    progress.route = route_;
    progress.enemyHp = enemyHp_;
    progress.pendingEvent = pendingEvent_;
    saves_.save(player_, progress);
}

void Game::load() {
    Player loaded = player_;
    GameProgress progress;
    if (!saves_.load(loaded, progress)) {
        std::cout << "没有找到存档，请先保存。\n";
        return;
    }
    if (progress.stageIndex < 0 || progress.stageIndex > 10)
        throw std::runtime_error("存档房间编号无效。");
    const int id = progress.stageIndex + 1;
    const bool branch = campus::battleBranch(id) || campus::shopBranch(id);
    if (progress.formatVersion != 3 || id < 1 || id > 11 ||
        progress.currentRoomName != std::to_string(id) ||
        (branch ? (progress.route != 1 && progress.route != 2) : progress.route != 0))
        throw std::runtime_error("存档版本或房间进度不兼容，请使用本版存档。");
    const int maxHp = (!branch || progress.route == 1) ? campus::enemy(id).hp : 0;
    if (progress.pendingEvent < 0 || progress.pendingEvent > 8 || (progress.pendingEvent > 0 && (!branch || progress.route != 2)))
        throw std::runtime_error("存档事件无效。");
    if (progress.enemyHp < 0 || progress.enemyHp > maxHp)
        throw std::runtime_error("存档中的敌人血量无效。");
    // 读取不再触发事件、休息或战斗奖励。
    player_ = loaded;
    enterRoom(id);
    route_ = progress.route;
    if (maxHp > 0) runBattle(id != 2);
    enemyHp_ = progress.enemyHp;
    pendingEvent_ = progress.pendingEvent;
    std::cout << "已恢复玩家、房间和敌人血量。\n";
}

void Game::enterRoom(int id) {
    currentRoomId_ = id;
    route_ = 0;
    pendingEvent_ = 0;
    enemyName_.clear();
    enemyHp_ = enemyMaxHp_ = enemyAttack_ = enemyReward_ = 0;
}


void Game::chooseBranch1() {
    std::cout << "\n[选择路线] 1 直面学业挑战   2 校园奇遇   0 取消\n";
    const int choice = readChoice(0,2);
    if (choice == 0) return;
    enterRoom(currentRoomId_ + 1);
    route_ = choice;
    if (choice == 1) runBattle(false);
    else { pendingEvent_ = campus::eventId(); runEvent(); }
}
void Game::chooseBranch2() {
    std::cout << "\n[选择路线] 1 教育超市   2 校园奇遇   0 取消\n";
    const int choice = readChoice(0,2);
    if (choice == 0) return;
    enterRoom(currentRoomId_ + 1);
    route_ = choice;
    if (choice == 1) runShop();
    else { pendingEvent_ = campus::eventId(); runEvent(); }
}
int Game::readChoice(int min,int max) {
    return module5::CommandParser::readChoice(std::cin,std::cout,min,max);
}
void Game::runBattle(bool /*isBoss*/) {
    const auto e = campus::enemy(currentRoomId_);
    enemyName_ = e.name;
    enemyHp_ = enemyMaxHp_ = e.hp;
    enemyAttack_ = e.attack;
    enemyReward_ = e.gold;
}
void Game::runEvent() {
    campus::event(pendingEvent_,player_);
    pendingEvent_ = 0;
}
void Game::runShop() {
    std::vector<Item> goods = {
        Item("续命咖啡","campus","恢复30精神状态",20,30),
        Item("期末救命包","campus","恢复55精神状态",28,45),
        Item("抗压笔记","campus","增加30抗压",25,0,30),
        Item("学霸笔记","campus","学力增加5",30,0,0,5),
        Item("幸运橡皮","campus","恢复30精神状态、增加30抗压、学力增加5",60,30,30,5),
        Item("Tired Bull功能饮料","campus","恢复30精神状态、增加30抗压",40,30,30),
        Item("传奇盲盒","campus","有概率开出隐藏奖励：2500元 哦！",300,1,1,1),
        Item("神话盲盒","campus","有概率开出隐藏奖励：750元 哦！",100,1,1,1),
        Item("史诗盲盒","campus","有概率开出隐藏奖励：500元 哦！",75,1,1,1),
        Item("稀有盲盒","campus","有概率开出隐藏奖励：300元 哦！",50,1,1,1),
        Item("普通盲盒","campus","有概率开出隐藏奖励：50元 哦！",10,1,1,1)
    };
    std::shuffle(goods.begin(),goods.end(),campus::random());
    goods.resize(4);
    while (!goods.empty()) {
        std::cout << "\n[教育超市] 生活费：" << player_.getGold() << "元\n";
        for (std::size_t i=0;i<goods.size();++i)
            std::cout << i+1 << ". " << goods[i].name << " 价格：" << goods[i].price << "元 " << goods[i].description << '\n';
        std::cout << "0 离开；购买后在背包中使用。\n";
        const int choice = readChoice(0,static_cast<int>(goods.size()));
        if(choice==0) return;
        const Item item = goods.at(choice-1);
        if(!player_.spendGold(item.price)) { std::cout << "生活费不足。\n"; continue; }
        player_.addItem(item);
        goods.erase(goods.begin()+choice-1);
    }
    std::cout << "商品已售罄。\n";
}
void Game::runRest() {
    player_.heal(40);
    std::cout << "休整完成，恢复40精神状态（不超过上限）。输入4挑战期末大考。\n";
}
