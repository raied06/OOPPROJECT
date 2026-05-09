#pragma once
#include "DamageableEntity.h"
#include "EnemyAIState.h"
#include "Level.h"
#include <SFML/Graphics.hpp>

// Forward declarations — full headers included in Enemy.cpp
class Player;
class EntityManager;
class Weapon;
                                                    // ABSTRACT BASE CLASS for all Enemies

/*  Inherits features from DamageableEntity like health points, color flickering etc.
    EnemyAIState is owned by it (composition)
    Weapons are owned by it (composition)
    Has its own mechanics (physics implementation i.e., gravity, collision etc)
    Child class will use it, pass their specific sprites to it and their own hp, dimensions etc. */

class Enemy : public DamageableEntity
{
protected:
// NON-OWNED CONTEXT POINTERS
    const Level* level; // Used to verify collisions
    Player* player; // Used to decide motion of Enenmies towards player, their shooting
    EntityManager* entities;  // Entity array to spawn projectiles

    EnemyAIState* currentState; // Composed here
    Weapon* weapon; // Composed here

// SPRITE (VISUALS)
    bool        hasTexture;
    sf::Texture texture;
    sf::Sprite  sprite;
    sf::Color   placeholderColor; // drawn when no sprite is available

    bool  facingRight;
    float baseScaleX;
    float baseScaleY;

// MOVEMENT VARS (FOR PHYSICS IMPLEMENTATION)
    bool  onGround;
    float moveSpeed;
    float gravity;
    float maxFallSpeed;

// AI FEATURES
    float detectionRange;
    float attackRange;

    // ── Internal helpers ─────────────────────────────────────────────────────
    // Try to load a texture and compute hitbox width from it.
    // Returns false and falls back to placeholder rectangle if the file
    // does not exist — enemies will still function fully.
    bool tryLoadTexture(const char* path, float targetH);

    void resolveHorizontal();
    void resolveVertical();

    // Changing state to a new one (Patrol changed to chase etc)
    void transitionTo(EnemyAIState* newState);

public:
    // targetH      : desired rendered height in pixels (width auto-calculated from texture)
    // spritePath   : relative path; if the file is missing a coloured box is drawn instead
    // fallbackColor: the colour of that placeholder box
    Enemy(float x, float y, float targetH,
          int hp,
          const char* spritePath,
          sf::Color fallbackColor,
          const Level* lvl,
          Player* p,
          EntityManager* em);

    virtual ~Enemy();

// Cannot copy objects
    Enemy(const Enemy&)            = delete;
    Enemy& operator=(const Enemy&) = delete;

    virtual void update(float dt) override;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) override;

    // Accepts hits from player projectiles only.
    virtual bool receiveProjectileHit(int damage, bool fromPlayer) override;

    // Will be called by AI state
    void moveLeft();
    void moveRight();
    void moveTowardPlayer();
    void stopMoving();
    void fireWeapon();

    float distanceToPlayer() const;
// GETTERS
    float getDetectionRange() const;
    float getAttackRange() const;
    bool isOnGround() const;
    Player* getPlayer() const;
// Retargets AI to the newly spawned player
    virtual void onPlayerRespawn(Player* newPlayer) override { player = newPlayer; }
};
