#include "Game.h"

#include <iostream>

int main()
{
    Game game({10, 10}, 5); // 10x10 field and 5 mines and flags
    game.start();

    std::cout << "Press enter to continue!\n";
    std::cin.ignore();
    
    return 0;
}
