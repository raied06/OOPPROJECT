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

    // ── Scoring ──────────────────────────────────────────────────────────────
    int   score;
    int   comboCount;          // how many extra kills in the current chain
    float comboTimer;          // > 0 = chain window still open
    static constexpr float COMBO_WINDOW = 1.5f;
    static constexpr int   COMBO_CAP    = 8;     // multiplier caps here

    // ── Level progression ────────────────────────────────────────────────────
    int   currentLevel;         // 1 or 2
    float levelTransitionTimer; // > 0 = "Level N Complete!" overlay is showing
    static constexpr int   MAX_LEVEL                = 2;
    static constexpr float LEVEL_TRANSITION_DURATION = 2.0f;
    // Trigger threshold — world is 12800 px wide; clear when player hits this.
    static constexpr float LEVEL_END_X              = 12500.0f;

    // HUD
    sf::Font hudFont;
    sf::Text scoreText;
    sf::Text comboText;
    sf::Text grenadeText;        // bottom-left
    sf::Text levelText;          // bottom-middle
    sf::Text levelCompleteText;  // big center overlay during transition
    bool     hudFontLoaded;

    // Applies the combo multiplier and refreshes the HUD strings.
    void awardKill(int baseValue);

    // Clears the world of enemies/projectiles, resets player/camera to start,
    // spawns the chosen level's enemy layout, and updates the level HUD label.
    void startLevel(int n);

    // Per-level enemy placements — Level 2 keeps Level 1's tile layout but
    // packs in more enemies for difficulty progression.
    void spawnLevel1Enemies();
    void spawnLevel2Enemies();

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
