//#pragma once
//#include "GameState.h"
//#include <SFML/Graphics.hpp>
//#include <iostream>
//
//class PlayState : public GameState {
//private:
//    // Textures and Sprites
//    sf::Texture playerTex;
//    sf::Sprite playerSprite;
//    sf::Texture wallTex1;
//    sf::Sprite wallSprite1;
//
//    // Level Data
//    char** lvl;
//    int height;
//    int width;
//    int cell_size;
//
//    // Player Physics & Stats
//    float player_x;
//    float player_y;
//    float playerWidth;
//    float playerHeight;
//    float max_speed;
//    float gravity;
//    float velocityX;
//    float velocityY;
//    float acceleration;
//
//    // Jump mechanics
//    bool isOnGround;
//    float initialJumpSpeed;
//
//public:
//    PlayState();
//    ~PlayState() override;
//
//    void handleInput(sf::Event& event, sf::RenderWindow& window) override; // Overriden function from GameState class
//    void update(float dt) override;
//    void render(sf::RenderWindow& window) override;
//};
#pragma once
#include "GameState.h"
#include "Level.h"
#include "Player.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class PlayState : public GameState
{
private:
    Level* level;    // owns the world
    EntityManager* entities; // owns all entities including player

    // Non-owning convenience pointer to the player entity so we can
    // follow it with the camera without searching the manager every frame.
    // EntityManager handles the actual delete - do NOT delete this here.
    Player* player;

    sf::Texture bgTex;
    sf::Sprite  bgSprite;

    float cameraX;
    float cameraY;

public:
    PlayState();
    ~PlayState() override;

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};