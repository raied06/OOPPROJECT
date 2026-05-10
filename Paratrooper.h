#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// Paratrooper  —  drops from the top of the screen under a parachute
// ═════════════════════════════════════════════════════════════════════════════
// Stats:
//   HP            : 2
//   Weapon        : Pistol (infinite ammo, 1 dmg, 1.0 s cooldown)
//   Movement speed: 110 px/s  (light infantry once landed)
//   Detection     : 450 px
//   Attack range  : 250 px
//
// Two-phase behaviour:
//   Phase 1 — ParachuteState: reduced gravity (150 px/s²) while descending.
//   Phase 2 — Normal AI (Patrol → Chase → Attack) once onGround.
//
// Spawned by PlayState at the top of the screen at a random X above the player.
// ─────────────────────────────────────────────────────────────────────────────
class Paratrooper : public Enemy
{
private:
    bool hasLanded;

    static constexpr float PARACHUTE_GRAVITY = 180.0f;  // slow fall
    static constexpr float GROUND_GRAVITY    = 1500.0f; // normal after landing

public:
    Paratrooper(float x, float y,
                const Level*   lvl,
                Player*        p,
                EntityManager* em);

    virtual ~Paratrooper() {}

    // Override update to switch gravity on first landing.
    virtual void update(float dt) override;

    int getScoreValue() const override { return 150; }
};
