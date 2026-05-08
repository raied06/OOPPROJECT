#pragma once
#include "Entity.h"
#include "Level.h"

// ═════════════════════════════════════════════════════════════════════════════
// Projectile  —  abstract base for all projectiles (per UML)
// ═════════════════════════════════════════════════════════════════════════════
// Extends Entity directly (not DamageableEntity — projectiles can't be damaged,
// they only deal damage).
//
// Holds damage value and a fromPlayer flag so the collision system knows
// which camp a bullet belongs to (friendly-fire prevention).
//
// Subclasses implement movement and self-deactivation on wall/bounds hit.
// ─────────────────────────────────────────────────────────────────────────────
class Projectile : public Entity
{
protected:
    int          damage;
    bool         fromPlayer; // true = player bullet, false = enemy bullet
    const Level* level;      // non-owning; used for tile collision checks

public:
    Projectile(float x, float y, float w, float h,
               int damage, bool fromPlayer, const Level* lvl);

    virtual ~Projectile() {}

    int  getDamage()    const { return damage; }
    bool isFromPlayer() const { return fromPlayer; }

    // Still pure virtual.
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) = 0;
};
