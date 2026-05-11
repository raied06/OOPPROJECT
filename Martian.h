#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// Martian  —  ground-walking alien enemy
// ═════════════════════════════════════════════════════════════════════════════
// Stats:
//   HP            : 3
//   Weapon        : Energy Beam (fast straight shot, 2 dmg, 0.8 s cooldown)
//   Movement speed: 90 px/s
// ─────────────────────────────────────────────────────────────────────────────
class Martian : public Enemy
{
public:
    Martian(float x, float y,
            const Level*   lvl,
            Player*        p,
            EntityManager* em);

    virtual ~Martian() {}

    int getScoreValue() const override { return 500; }
};
