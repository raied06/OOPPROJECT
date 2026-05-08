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
    Level* level;    // This basically means that the Level object is not born yet, 
                  // it will born when constructor is called. + (composition relation) we control its birth as well as death
        // 1. This pointer allow to pass any value for the dimensions of grid.
        // 2. Interchange levels at runtime, 
        // 3. We can replace the old level with a new one

    EntityManager* entities; // owns all entities including player
    Player* player;

    sf::Texture bgTex;
    sf::Sprite  bgSprite;

    // Camera (Top left corner of visible screen)
    float cameraX;
    float cameraY;

public:
    PlayState();
    ~PlayState() override;

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};