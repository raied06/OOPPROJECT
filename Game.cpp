#include "Game.h"
#include "PlayState.h"

Game::Game() : window(sf::VideoMode(1600, 900), "Metal Slug - Phase 1", sf::Style::Close) {
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // Boot up the first state
    stateManager.pushState(new PlayState());
}

void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        GameState* currentState = stateManager.getCurrentState();

        // 1. Handle Events (Single triggers like closing the window)
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (currentState) {
                currentState->handleInput(event, window);
            }
        }

        // 2. Update Logic (Movement, Physics, etc.)
        if (currentState) {
            currentState->update(dt);
        }

        // 3. Render
        window.clear();
        if (currentState) {
            currentState->render(window);
        }
        window.display();
    }
}