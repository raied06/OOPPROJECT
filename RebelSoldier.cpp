#include "RebelSoldier.h"
#include "ProjectileWeapon.h"

// ── Tuning constants ──────────────────────────────────────────────────────────
static constexpr float REBEL_TARGET_H     = 150.0f; // rendered height in pixels — matches player
static constexpr int   REBEL_HP           = 2;
static constexpr float REBEL_MOVE_SPEED   = 120.0f; // px / s
static constexpr float REBEL_DETECT_RANGE = 450.0f; // px — patrol → chase
static constexpr float REBEL_ATTACK_RANGE = 250.0f; // px — chase → attack

// Pistol constants
static constexpr float PISTOL_COOLDOWN    = 1.2f;   // seconds between shots
static constexpr int   PISTOL_DAMAGE      = 1;
static constexpr float PISTOL_SPEED       = 500.0f; // px / s
// ─────────────────────────────────────────────────────────────────────────────

RebelSoldier::RebelSoldier(float x, float y,
                             const Level*   lvl,
                             Player*        p,
                             EntityManager* em)
    : Enemy(x, y,
            REBEL_TARGET_H,
            REBEL_HP,
            "Sprites/Enemies/rebel_soldier.png", // falls back to placeholder if missing
            sf::Color(139, 90, 43),               // earthy-brown placeholder box
            lvl, p, em)
{
    // Override default Enemy stats.
    moveSpeed      = REBEL_MOVE_SPEED;
    detectionRange = REBEL_DETECT_RANGE;
    attackRange    = REBEL_ATTACK_RANGE;

    // Arm the rebel with a pistol (infinite ammo, straight projectile).
    weapon = new ProjectileWeapon(
        PISTOL_COOLDOWN,
        PISTOL_DAMAGE,
        -1,                          // infinite ammo
        PISTOL_SPEED,
        false,                       // straight shot — no gravity
        sf::Color(255, 230, 50),     // yellow bullet
        lvl
    );
}
