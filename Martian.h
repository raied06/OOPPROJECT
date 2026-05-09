#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// Martian  —  two-phase alien enemy
// ═════════════════════════════════════════════════════════════════════════════
// Stats:
//   HP            : 3
//   Weapon        : Energy Beam (fast straight shot, 2 dmg, 0.8 s cooldown)
//   Movement speed: 90 px/s (ground phase)
//
// Phase 1 — MartianFlyState:
//   The pod drifts horizontally toward the player while hovering.
//   After ~4 seconds it descends and lands.
//   During flight gravity is near-zero so it stays airborne.
//
// Phase 2 — Normal AI (Patrol → Chase → Attack):
//   Once grounded the Martian walks and fires its energy beam at the player.
// ─────────────────────────────────────────────────────────────────────────────
class Martian : public Enemy
{
private:
    static constexpr float FLY_GRAVITY    = 30.0f;   // near-weightless in pod
    static constexpr float GROUND_GRAVITY = 1500.0f;
    bool hasLanded;

public:
    Martian(float x, float y,
            const Level*   lvl,
            Player*        p,
            EntityManager* em);

    virtual ~Martian() {}

    // Override to switch gravity once landed.
    virtual void update(float dt) override;
};
