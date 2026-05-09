#pragma once
#include "Entity.h"
#include "Level.h"

class EntityManager; // forward declaration — full include in Projectile.cpp

// ═════════════════════════════════════════════════════════════════════════════
// Projectile  —  abstract base for all projectiles (per UML)
// ═════════════════════════════════════════════════════════════════════════════
// Extends Entity directly (not DamageableEntity — projectiles can't be damaged,
// they only deal damage).
//
// Each projectile holds a non-owning EntityManager* and calls
// checkEntityCollisions() from its update(). This keeps all collision logic
// inside the projectile hierarchy via virtual dispatch — no dynamic_cast needed.
// ─────────────────────────────────────────────────────────────────────────────
class Projectile : public Entity
{
protected:
    int            damage;
    bool           fromPlayer;    // true = player bullet, false = enemy bullet
    const Level*   level;         // non-owning
    EntityManager* entities;      // non-owning — used to iterate hit targets

    // Iterates all active entities; calls receiveProjectileHit() on each
    // overlapping one. Deactivates self on the first accepted hit.
    // Pure virtual dispatch handles which entity type accepts which bullet.
    void checkEntityCollisions();

public:
    Projectile(float x, float y, float w, float h,
               int damage, bool fromPlayer,
               const Level* lvl, EntityManager* em);

    virtual ~Projectile() {}

    int  getDamage()    const { return damage; }
    bool isFromPlayer() const { return fromPlayer; }

    // Still pure virtual.
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) = 0;
};
