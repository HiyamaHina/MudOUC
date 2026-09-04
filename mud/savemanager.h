#pragma once
#include <string>
class Player;
class Game;

class SaveManager
{
private:
    std::string filePath;
public:
    SaveManager(std::string path);
    void save(Game* game);
    Game* load();
    void savePlayer(Player* p);
    void saveRoom();
};
