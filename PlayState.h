#pragma once
#include "GameState.h"
#include "Level.h"
#include "Player.h"
#include "EriPlayer.h"
#include "MarcoPlayer.h"
#include "TarmaPlayer.h"
#include "FiolinaPlayer.h"
#include "Enemy.h"
#include "RebelSoldier.h"
#include "ShieldedSoldier.h"
#include "BazookaSoldier.h"
#include "GrenadeSoldier.h"
#include "Paratrooper.h"
#include "Zombie.h"
#include "MummyWarrior.h"
#include "Martian.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class PlayState : public GameState
{
private:
    Level*         level;
    EntityManager* entities;
    Player*        player;

    sf::Texture bgTexPlain;
    sf::Texture bgTexAerial;
    sf::Texture bgTexAquatic;
    sf::Sprite  bgSprite;

    float cameraX;
    float cameraY;

    int   lives;
    float respawnTimer;
    float spawnX, spawnY;      // screen-space offsets from camera top-left
    Player* respawnPrototype;  // factory-only; never added to EntityManager

    static constexpr float RESPAWN_DELAY = 2.0f;

    void respawnPlayer();

    void spawnRebelBatch      (float x, float y, int count);
    void spawnShieldedBatch   (float x, float y, int count);
    void spawnBazookaBatch    (float x, float y, int count);
    void spawnGrenadeBatch    (float x, float y, int count);
    void spawnParatrooperBatch(float x, float y, int count);
    void spawnZombieBatch     (float x, float y, int count);
    void spawnMummyBatch      (float x, float y, int count);
    void spawnMartianBatch    (float x, float y, int count);

public:
    PlayState();
    ~PlayState() override;

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};
