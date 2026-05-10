#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// ShieldedSoldier  —  tanky infantry with a front-facing shield
// ═════════════════════════════════════════════════════════════════════════════
// Stats:
//   HP            : 5
//   Weapon        : Pistol (infinite ammo, 1 dmg, 1.4 s cooldown)
//   Movement speed: 90 px/s  (slower — heavy shield)
//   Detection     : 400 px
//   Attack range  : 220 px
//
// Shield mechanic (takeDamage override):
//   Bullets that hit the FRONT of the soldier (facingRight && hitFromLeft,
//   or !facingRight && hitFromRight) are blocked — 0 damage.
//   Hits from ABOVE (vertical projectiles), BEHIND, or explosion damage
//   bypass the shield and deal full damage.
//
//   For simplicity in this phase the shield direction is inferred from the
//   projectile's fromPlayer flag combined with relative X position:
//   if the player is in front of the shield, the shot is blocked.
// ─────────────────────────────────────────────────────────────────────────────
class ShieldedSoldier : public Enemy
{
public:
    ShieldedSoldier(float x, float y,
                    const Level*   lvl,
                    Player*        p,
                    EntityManager* em);

    virtual ~ShieldedSoldier() {}

    // Override to apply shield logic.
    // Only player-sourced hits from the front are blocked.
    virtual void takeDamage(int amount) override;

    // Marco's piercing knife bypasses the shield and instakills.
    virtual bool receiveMeleeHit(int damage, bool fromPlayer) override;

    int getScoreValue() const override { return 200; }

private:
    // Returns true when the player is currently standing in front of the shield.
    bool playerIsInFront() const;
};
