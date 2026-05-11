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
    Player* player; // Used to track player (enemy's charge towards it)
    EntityManager* entities;  // Entity array to spawn projectiles

    EnemyAIState* currentState; // Composition
    Weapon* weapon; // COMPOSITION

// SPRITE (VISUALS)
    sf::Texture texture;
    sf::Sprite  sprite;

    bool  facingRight;
    float baseScaleX;
    float baseScaleY;

// MOVEMENT VARIABLES (FOR PHYSICS IMPLEMENTATION)
    bool  onGround;
    float moveSpeed;
    float gravity;
    float maxFallSpeed;

// AI FEATURES
    float detectionRange;
    float attackRange;

    bool loadTexture(float targetH);

    void resolveHorizontal();
    void resolveVertical();

    // Changing state to a new one (Patrol changed to chase etc)
    void transitionTo(EnemyAIState* newState);

public:
    Enemy(float x, float y, float targetH,
          int hp,
          const char*  spritePath,
          const Level* lvl,
          Player* p,
          EntityManager* em);

    virtual ~Enemy();

// If we try to cipy an enemy into the other, if one dies, with it the composed EnemyAIState and Weapon will also be deleted, and when
// the copied Enemy will try to use it, but it is deleted.
    Enemy(const Enemy&) = delete;
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
    float getWidth() const;
    float getVelocityY() const;

    virtual void onPlayerRespawn(Player* newPlayer) override; 
    virtual void applyScreenClamp(float cameraX) override;
};
