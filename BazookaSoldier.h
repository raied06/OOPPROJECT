#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// BazookaSoldier  —  slow-firing rocket trooper
// ═════════════════════════════════════════════════════════════════════════════
// Stats:
//   HP            : 2
//   Weapon        : Rocket Launcher (1 rocket, 3.0 s cooldown, 5 dmg, ballistic)
//   Movement speed: 70 px/s  (heavy weapon slows movement)
//   Detection     : 500 px  (engages from further away)
//   Attack range  : 350 px  (fires from a safe distance)
//
// Behaviour: identical state-machine to RebelSoldier (Patrol → Chase → Attack)
// but fires a slow, arc-falling rocket instead of a pistol bullet.
// ─────────────────────────────────────────────────────────────────────────────
class BazookaSoldier : public Enemy
{
public:
    BazookaSoldier(float x, float y,
                   const Level*   lvl,
                   Player*        p,
                   EntityManager* em);

    virtual ~BazookaSoldier() {}
};
