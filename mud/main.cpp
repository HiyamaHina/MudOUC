#include <iostream>
#include "Player.h"
#include "Room.h"
#include "CommandParser.h"

int main()
{
    std::cout << "===== 获币战争 =====" << std::endl;
    Player* player = new Player();
    StartRoom start;
    start.enter(player);

    delete player;
    return 0;
}
