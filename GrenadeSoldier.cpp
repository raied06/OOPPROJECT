#include "GrenadeSoldier.h"
#include "ProjectileWeapon.h"

// ── Tuning constants ──────────────────────────────────────────────────────────
static constexpr float GRENADE_TARGET_H     = 150.0f;
static constexpr int   GRENADE_HP           = 2;
static constexpr float GRENADE_MOVE_SPEED   = 100.0f;
static constexpr float GRENADE_DETECT_RANGE = 420.0f;
static constexpr float GRENADE_ATTACK_RANGE = 300.0f;

// Grenade: medium damage, arc trajectory (ballistic), moderate cooldown
static constexpr float GRENADE_COOLDOWN = 2.5f;
static constexpr int   GRENADE_DAMAGE   = 4;
static constexpr float GRENADE_SPEED    = 380.0f; // horizontal component
// ─────────────────────────────────────────────────────────────────────────────

GrenadeSoldier::GrenadeSoldier(float x, float y,
                               const Level*   lvl,
                               Player*        p,
                               EntityManager* em)
    : Enemy(x, y,
            GRENADE_TARGET_H,
            GRENADE_HP,
            "Sprites/Enemies/GrenadeSoldier.png",
            lvl, p, em)
{
    moveSpeed      = GRENADE_MOVE_SPEED;
    detectionRange = GRENADE_DETECT_RANGE;
    attackRange    = GRENADE_ATTACK_RANGE;

    // Hand grenade — ballistic arc, notable damage, slow rate of fire.
    weapon = new ProjectileWeapon(
        GRENADE_COOLDOWN,
        GRENADE_DAMAGE,
        -1,           // infinite supply
        GRENADE_SPEED,
        true,         // ballistic — gravity pulls the grenade down
        sf::Color(50, 180, 50),   // green grenade
        lvl
    );
}
