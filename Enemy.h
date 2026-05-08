#pragma once
#include "DamageableEntity.h"
#include "EnemyAIState.h"
#include "Level.h"
#include <SFML/Graphics.hpp>

// Forward declarations — full headers included in Enemy.cpp
class Player;
class EntityManager;
class Weapon;

// ═════════════════════════════════════════════════════════════════════════════
// Enemy  —  abstract base for all enemy types
// ═════════════════════════════════════════════════════════════════════════════
// Inherits HP / damage flash from DamageableEntity.
// Owns an EnemyAIState* (state-pattern AI) and a Weapon*.
// Owns its own physics (gravity + tile collision) — separate from Soldier so
// the two hierarchies stay independent per the UML.
//
// Concrete subclasses (RebelSoldier, etc.) call this constructor with their
// specific sprite path, colour fallback, HP, and target height, then set their
// own stats (moveSpeed, detectionRange, etc.) and hand in their weapon.
// ─────────────────────────────────────────────────────────────────────────────
class Enemy : public DamageableEntity
{
protected:
    // ── Non-owning context pointers ──────────────────────────────────────────
    const Level*   level;     // tilemap — for collision queries
    Player*        player;    // AI target — for distance / direction
    EntityManager* entities;  // entity pool — for spawning projectiles

    // ── AI ───────────────────────────────────────────────────────────────────
    EnemyAIState* currentState; // OWNED — deleted on transition / destruction

    // ── Weapon ───────────────────────────────────────────────────────────────
    Weapon* weapon; // OWNED — set by subclass constructor

    // ── Visuals ──────────────────────────────────────────────────────────────
    bool        hasTexture;
    sf::Texture texture;
    sf::Sprite  sprite;
    sf::Color   placeholderColor; // drawn when no sprite is available

    bool  facingRight;
    float baseScaleX;
    float baseScaleY;

    // ── Physics ──────────────────────────────────────────────────────────────
    bool  onGround;
    float moveSpeed;
    float gravity;
    float maxFallSpeed;

    // ── AI tuning (overrideable by subclasses) ───────────────────────────────
    float detectionRange; // patrol → chase threshold (pixels)
    float attackRange;    // chase  → attack threshold (pixels)

    // ── Internal helpers ─────────────────────────────────────────────────────
    // Try to load a texture and compute hitbox width from it.
    // Returns false and falls back to placeholder rectangle if the file
    // does not exist — enemies will still function fully.
    bool tryLoadTexture(const char* path, float targetH);

    void resolveHorizontal();
    void resolveVertical();

    // Swap to a new AI state, calling exit() on the old and enter() on the new.
    void transitionTo(EnemyAIState* newState);

public:
    // targetH      : desired rendered height in pixels (width auto-calculated from texture)
    // spritePath   : relative path; if the file is missing a coloured box is drawn instead
    // fallbackColor: the colour of that placeholder box
    Enemy(float x, float y, float targetH,
          int hp,
          const char*  spritePath,
          sf::Color    fallbackColor,
          const Level* lvl,
          Player*      p,
          EntityManager* em);

    virtual ~Enemy();

    // Non-copyable (owns raw pointers)
    Enemy(const Enemy&)            = delete;
    Enemy& operator=(const Enemy&) = delete;

    // ── Entity interface ─────────────────────────────────────────────────────
    virtual void update(float dt) override;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) override;

    // ── Called by AI states ──────────────────────────────────────────────────
    void moveLeft();
    void moveRight();
    void moveTowardPlayer();
    void stopMoving();
    void fireWeapon();

    // ── Queried by AI states ─────────────────────────────────────────────────
    float   distanceToPlayer()    const;
    float   getDetectionRange()   const { return detectionRange; }
    float   getAttackRange()      const { return attackRange; }
    bool    isOnGround()          const { return onGround; }
    Player* getPlayer()           const { return player; }
};
