#include "CommandParser.h"
#include "Player.h"
#include <iostream>

void CommandParser::parse(std::string input, Player* p)
{
    if(input == "status")
    {
        p->showStatus();
    }
    else
    {
        std::cout << "未知指令" << std::endl;
    }
}
