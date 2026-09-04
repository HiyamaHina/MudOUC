#pragma once
#include <string>
class Player;

class CommandParser
{
public:
    void parse(std::string input, Player* p);
};
