#pragma once
#include "GameState.h"
#include "GameStateManager.h"
#include <SFML/Graphics.hpp>

class MenuState : public GameState
{
private:
    GameStateManager& gsm;
    sf::RenderWindow& windowRef;

    sf::Texture bgTexture;
    sf::Sprite  bgSprite;

    sf::Font    font;
    sf::Text    msgText;       // "THIS MODE DOES NOT EXIST YET"
    float       closeTimer;    // counts down after campaign choice; closes window at 0

public:
    MenuState(GameStateManager& gsm, sf::RenderWindow& window);

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};
