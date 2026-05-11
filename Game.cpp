#include "Game.h"
#include "MenuState.h"

Game::Game() : window(sf::VideoMode(1600, 900), "METAL SLUG", sf::Style::Close) {
    window.setFramerateLimit(60); // Upper cap for frames (max fps that we can get are 60, not more than this)
    window.setVerticalSyncEnabled(true);

    // Show the main menu first; it will push PlayState when the player chooses.
    stateManager.pushState(new MenuState(stateManager, window));
}

void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        // dt basically calculates that exactly how much time has passed beetween the last and the current frame.
        // Then, we multiply the speed by dt so movement is smooth regardless of hardware and game does not get laggy if frames drop below 60.

        GameState* currentState = stateManager.getCurrentState();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (currentState) {
                currentState->handleInput(event, window);
            }
        }

        if (currentState) {
            currentState->update(dt);
        }

        window.clear();
        if (currentState) {
            currentState->render(window);
        }
        window.display();
    }
}