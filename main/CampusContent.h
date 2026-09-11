#pragma once
#include "../Player.h"
#include "../module5/CommandParser.h"
#include <iostream>
#include <random>

namespace campus {
struct EnemyInfo { const char* name; int hp; int attack; int gold; };
inline EnemyInfo enemy(int room) {
    switch (room) {
    case 2: return {"大学第一节课程",45,8,12};
    case 3: return {"第一份作业",55,10,15};
    case 4: return {"随堂测验",100,14,40};
    case 6: return {"小组作业",75,14,20};
    case 7: return {"期中考试",115,20,60};
    case 9: return {"水课论文",90,17,25};
    case 11: return {"期末大考",160,25,100};
    default: return {"",0,0,0};
    }
}
inline bool battleBranch(int id) { return id==2 || id==3 || id==6 || id==9; }
inline bool shopBranch(int id) { return id==5 || id==8; }
inline std::mt19937& random() { static std::mt19937 generator(std::random_device{}()); return generator; }
inline int eventId() { return std::uniform_int_distribution<int>(1,10)(random()); }
inline void event(int id, Player& p) {
    const char* names[] = {"", "校园商人（一）", "校园忽悠（二）", "焚化工", "自灭者的火种",
                           "学业性梦境", "来交换礼物吧", "校园好人（三）", "无尽黑暗","没有喘息空间的课表","超级校园80","好老师与好课"};
    std::cout << "\n[校园奇遇] " << names[id] << '\n';
    switch(id) {
    case 1: std::cout << "1 金属许愿瓶：20元换10抗压\n2 银矿许愿瓶：40元换25抗压\n3 离开\n"; break;
    case 2: std::cout << "1 琥珀许愿匣：10金币，捉摸不透的祝福\n2 超钛许愿匣：10金币，璀璨华丽的祝福\n3 离开\n"; break;
    case 3: std::cout << "1 奉献自己：承受10压力，学力+4，灵光+5%\n2 承受十克拉的垃圾：获得10元，超常发挥+10%\n"; break;
    case 4: std::cout << "1 接受火种：精神状态减半，学力翻倍，灵光+10%\n2 拒绝：获得30元\n"; break;
    case 5: std::cout << "1 继续梦境：学力减少30%，获得40元\n2 回去上学：获得18元\n"; break;
    case 6: std::cout << "1 祝福交换：精神状态减半，获得等额抗压\n2 离开\n"; break;
    case 7: std::cout << "1 钻石盒：5元，精神状态上限+20%\n2 原矿盒：5元，学力增加15%\n3 离开\n"; break;
    case 8: std::cout << "1 前往黑暗：损失一半生活费\n2 对抗引力：承受当前精神状态60%的压力\n"; break;
    case 9: std::cout << "1 坚持从早八撑到晚九：精神状态减少30%\n2 逃课爽玩：恢复20%上限的精神状态，学力降低30%\n"; break;
    case 10: std::cout << "1 被抢走巨额金币\n2 被揍到昏天黑地\n"; break;
    case 11: std::cout << "1 积极互动：学力+20%\n2 摸鱼度日：精神状态上限+15%\n"; break;  
    }
    // 先读取选择再修改角色；输入中断时事件保持待处理状态。
    const int c = module5::CommandParser::readChoice(std::cin,std::cout,1,(id==1||id==2||id==7)?3:2);
    const auto pay = [&](int amount) {
        if (p.spendGold(amount)) return true;
        std::cout << "金币不足，没有购买。\n"; return false;
    };
    switch(id) {
    case 1: if(c==1 && pay(20)) p.addShield(10); else if(c==2 && pay(40)) p.addShield(25); break;
    case 2: if(c==1 && pay(10)) p.addShield(10); else if(c==2 && pay(10)) { std::cout<<"上当了！\n"; p.takeDamage(10); } break;
    case 3: if(c==1) { p.takeDamage(10); p.setBaseAttack(p.getBaseAttack()+4); p.addCR(5); } else { p.addGold(10); p.addCD(10); } break;
    case 4: if(c==1) { p.setHp(p.getHp()/2); p.setBaseAttack(p.getBaseAttack()*2); p.addCR(10); } else p.addGold(30); break;
    case 5: if(c==1) { p.setBaseAttack(p.getBaseAttack()*70/100); p.addGold(40); } else p.addGold(18); break;
    case 6: if(c==1) { int sacrifice=p.getHp()/2; p.setHp(p.getHp()-sacrifice); p.addShield(sacrifice); } break;
    case 7: if(c==1 && pay(5)) p.addMaxHp(p.getMaxHp()*20/100); else if(c==2 && pay(5)) p.setBaseAttack(p.getBaseAttack()+p.getAttack()*15/100); break;
    case 8: if(c==1) p.spendGold(p.getGold()/2); else p.takeDamage(p.getHp()*60/100); break;
    case 9: if(c==1) {int sacrifice=p.getHp()*3/10; p.setHp(p.getHp()-sacrifice);}else{ p.setBaseAttack(p.getBaseAttack()*70/100); p.heal(p.getMaxHp()*20/100); } break;
    case 10: if(c==1) p.spendGold(p.getGold()*0.9); else {int sacrifice=p.getHp()-10; p.setHp(p.getHp()-sacrifice);} break;
    case 11: if(c==1) p.setBaseAttack(p.getBaseAttack()*120/100);else {p.addMaxHp(p.getMaxHp()*15/100);p.setHp(p.getHp()*1.15);} break; 
    }
    std::cout << "奇遇结束。\n";
}
}

