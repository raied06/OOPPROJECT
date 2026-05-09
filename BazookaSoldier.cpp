#include "BazookaSoldier.h"
#include "ProjectileWeapon.h"

// ── Tuning constants ──────────────────────────────────────────────────────────
static constexpr float BAZOOKA_TARGET_H     = 150.0f; // matches player height
static constexpr int   BAZOOKA_HP           = 2;
static constexpr float BAZOOKA_MOVE_SPEED   = 70.0f;
static constexpr float BAZOOKA_DETECT_RANGE = 500.0f;
static constexpr float BAZOOKA_ATTACK_RANGE = 350.0f;

// Rocket launcher: slow, high-damage, ballistic arc
static constexpr float ROCKET_COOLDOWN = 3.0f;   // 3 seconds between shots
static constexpr int   ROCKET_DAMAGE   = 5;
static constexpr float ROCKET_SPEED    = 320.0f;  // slower projectile
// ─────────────────────────────────────────────────────────────────────────────

BazookaSoldier::BazookaSoldier(float x, float y,
                               const Level*   lvl,
                               Player*        p,
                               EntityManager* em)
    : Enemy(x, y,
            BAZOOKA_TARGET_H,
            BAZOOKA_HP,
            "Sprites/Enemies/bazooka_soldier.png",
            sf::Color(180, 60, 20),   // dark-orange placeholder
            lvl, p, em)
{
    moveSpeed      = BAZOOKA_MOVE_SPEED;
    detectionRange = BAZOOKA_DETECT_RANGE;
    attackRange    = BAZOOKA_ATTACK_RANGE;

    // Ballistic (gravity-affected) rocket — high damage, slow fire rate.
    weapon = new ProjectileWeapon(
        ROCKET_COOLDOWN,
        ROCKET_DAMAGE,
        -1,            // infinite ammo (re-loaded from back-line)
        ROCKET_SPEED,
        true,          // gravity affects trajectory
        sf::Color(255, 100, 30),  // orange rocket
        lvl
    );
}
