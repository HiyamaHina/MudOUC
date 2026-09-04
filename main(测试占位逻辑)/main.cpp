#include <iostream>
#include "Game.h"
#include<windows.h>
int main() {
    SetConsoleOutputCP(CP_UTF8);
    Game game;
    game.start();
    return 0;
}