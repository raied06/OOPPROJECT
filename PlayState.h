#pragma once
#include "GameState.h"
#include "Level.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "RebelSoldier.h"
#include "ShieldedSoldier.h"
#include "BazookaSoldier.h"
#include "GrenadeSoldier.h"
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

    // Lives / respawn
    int   lives;              // starts at 3; game over when it hits 0
    float respawnTimer;       // counts down after death; player respawns when it hits 0
    float spawnX, spawnY;     // fixed respawn position
    static constexpr float RESPAWN_DELAY = 2.0f; // seconds between death and respawn

    // Checks every active Projectile against valid targets each frame.
    // Player bullets → enemies; enemy bullets → player.
    void checkProjectileCollisions();

    // Re-creates the player entity and resets the camera to the spawn point.
    void respawnPlayer();

    // Spawns 'count' of the specified enemy type spaced 80px apart.
    void spawnRebelBatch   (float x, float y, int count);
    void spawnShieldedBatch(float x, float y, int count);
    void spawnBazookaBatch (float x, float y, int count);
    void spawnGrenadeBatch (float x, float y, int count);

public:
    PlayState();
    ~PlayState() override;

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};
