#include "BazookaSoldier.h"
#include "ProjectileWeapon.h"

static const float BAZOOKA_TARGET_H = 150.0f;
static const int   BAZOOKASOLDIER_HP = 2;
static const float BAZOOKA_MOVE_SPEED = 70.0f;
static const float BAZOOKA_DETECT_RANGE = 500.0f;
static const float BAZOOKA_ATTACK_RANGE = 350.0f;

// ROCKET STATS
static const float ROCKET_COOLDOWN = 3.0f;   // 3 seconds between shots
static const int ROCKET_DAMAGE = 5;
static const float ROCKET_SPEED = 380.0f;  // shorter projectile (steeper arc)

// ------------------------------------------------------------------------------------------------------=

BazookaSoldier::BazookaSoldier(float x, float y,
                               const Level*   lvl,
                               Player*        p,
                               EntityManager* em)
    : Enemy(x, y,
            BAZOOKA_TARGET_H,
            BAZOOKASOLDIER_HP,
            "Sprites/Enemies/bazooka_soldier.png",
            lvl, p, em)
{
    moveSpeed      = BAZOOKA_MOVE_SPEED;
    detectionRange = BAZOOKA_DETECT_RANGE;
    attackRange    = BAZOOKA_ATTACK_RANGE;

    // Ballistic Projectile Weapon so gravity will effect it
    weapon = new ProjectileWeapon(
        ROCKET_COOLDOWN,
        ROCKET_DAMAGE,
        -1,            // infinite ammo
        ROCKET_SPEED,
        true,          // gravity affects trajectory
        sf::Color(255, 100, 30),  // orange rocket
        lvl
    );
}

int BazookaSoldier::getScoreValue() const
{
    return 300;
}
