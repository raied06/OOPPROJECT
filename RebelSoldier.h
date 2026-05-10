#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// RebelSoldier  —  the most basic infantry enemy
// ═════════════════════════════════════════════════════════════════════════════
// Stats (per project spec):
//   HP            : 2
//   Weapon        : Pistol (infinite ammo, 1 damage, fires every 1.2 s)
//   Movement speed: 120 px/s
//   Spawn batches : 2 – 4 (handled by PlayState / level spawner)
//   Detection     : 450 px
//   Attack range  : 250 px
//
// Sprite: "Sprites/Enemies/rebel_soldier.png"
//         Falls back to an earthy-brown rectangle if the file is missing.
// ─────────────────────────────────────────────────────────────────────────────
class RebelSoldier : public Enemy
{
public:
    RebelSoldier(float x, float y,
                 const Level*   lvl,
                 Player*        p,
                 EntityManager* em);

    virtual ~RebelSoldier() {}

    int getScoreValue() const override { return 100; }
};
