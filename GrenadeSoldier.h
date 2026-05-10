#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// GrenadeSoldier  —  lobs hand grenades in a high arc
// ═════════════════════════════════════════════════════════════════════════════
// Stats:
//   HP            : 2
//   Weapon        : Hand Grenade (ballistic, 4 dmg, 2.5 s cooldown)
//   Movement speed: 100 px/s
//   Detection     : 420 px
//   Attack range  : 300 px  (keeps its distance, relies on arc fire)
//
// The grenade is a BallisticProjectile launched with an upward y-component so
// it travels in a proper arc even on flat ground.  This is handled inside
// ProjectileWeapon::fire() for the ballistic variant — the initial velocityY
// is set to -lobSpeed so the grenade rises before falling.
// ─────────────────────────────────────────────────────────────────────────────
class GrenadeSoldier : public Enemy
{
public:
    GrenadeSoldier(float x, float y,
                   const Level*   lvl,
                   Player*        p,
                   EntityManager* em);

    virtual ~GrenadeSoldier() {}

    int getScoreValue() const override { return 250; }
};
