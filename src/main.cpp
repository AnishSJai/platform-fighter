#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Game game;
    
    if (!game.init("Platform Fighter", 800, 600)) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return 1;
    }
    
    game.run();
    
    return 0;
}
