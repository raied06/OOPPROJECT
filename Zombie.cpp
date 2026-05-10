#include "Zombie.h"
#include "ProjectileWeapon.h"
#include "Player.h"

static constexpr float ZOMBIE_TARGET_H     = 150.0f;
static constexpr int   ZOMBIE_HP           = 5;
static constexpr float ZOMBIE_MOVE_SPEED   = 80.0f;
static constexpr float ZOMBIE_DETECT_RANGE = 500.0f;
static constexpr float ZOMBIE_ATTACK_RANGE = 240.0f;

Zombie::Zombie(float x, float y,
               const Level*   lvl,
               Player*        p,
               EntityManager* em)
    : Enemy(x, y,
            ZOMBIE_TARGET_H,
            ZOMBIE_HP,
            "Sprites/Enemies/Zombie.png",
            lvl, p, em),
      contactCooldown(0.0f)
{
    moveSpeed      = ZOMBIE_MOVE_SPEED;
    detectionRange = ZOMBIE_DETECT_RANGE;
    attackRange    = ZOMBIE_ATTACK_RANGE;

    // Slow pistol — zombies prefer to shuffle close and bite.
    weapon = new ProjectileWeapon(
        2.0f,                       // slow fire rate
        1,                          // damage
        -1,                         // infinite ammo
        420.0f,                     // slow bullet
        false,
        sf::Color(100, 220, 80),    // green bullet
        lvl
    );
}

void Zombie::update(float dt)
{
    if (!isActive) return;

    // ── Melee contact check ───────────────────────────────────────────────────
    // Runs before the normal AI update so the zombie can close in AND bite
    // in the same frame it reaches the player.
    if (contactCooldown > 0.0f)
        contactCooldown -= dt;

    if (player && player->getIsActive() && contactCooldown <= 0.0f) {
        // Distance between centres.
        float myMidX  = positionX + entityWidth  * 0.5f;
        float myMidY  = positionY + entityHeight * 0.5f;
        float plMidX  = player->getPosX() + player->getEntityWidth()  * 0.5f;
        float plMidY  = player->getPosY() + player->getEntityHeight() * 0.5f;
        float dx = myMidX - plMidX;
        float dy = myMidY - plMidY;
        float dist = dx * dx + dy * dy; // compare squared to avoid sqrt

        if (dist < CONTACT_RANGE * CONTACT_RANGE) {
            // Direct call — no cast needed, Player* inherits takeDamage.
            player->takeDamage(CONTACT_DAMAGE);
            contactCooldown = CONTACT_INTERVAL;
            // TODO: trigger TransformationState on player here once implemented.
        }
    }

    // ── Normal AI + physics ───────────────────────────────────────────────────
    Enemy::update(dt);
}
