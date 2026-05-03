#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class PlayState : public GameState {
private:
    // Textures and Sprites
    sf::Texture playerTex;
    sf::Sprite playerSprite;
    sf::Texture wallTex1;
    sf::Sprite wallSprite1;

    // Level Data
    char** lvl;
    int height;
    int width;
    int cell_size;

    // Player Physics & Stats
    float player_x;
    float player_y;
    float max_speed;
    float velocityX;
    float velocityY;
    float acceleration;

    // Jump mechanics
    float maxJumpHeight;
    float positionBeforeJump;
    float jumpStart;
    bool isJumping;
    bool fall;

public:
    PlayState();
    ~PlayState() override;

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};