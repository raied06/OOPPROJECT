#pragma once
#include <SFML/Graphics.hpp>
#include "GameStateManager.h"

class Game {
private:
    sf::RenderWindow window;
    GameStateManager stateManager;
    sf::Clock clock;

public:
    Game();
    void run();
};