#include <iostream>

#include "Game.hpp"

Game game;

int32_t main(int32_t argc, char** argv) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [map]" << std::endl;
        return 1;
    }

    try {
        if (argc == 2)
            game.init(argv[1]);
        else
            game.init("maps/map1.txt");
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    game.loop();

    return 0;
}
