#include "Martian.h"
#include "ProjectileWeapon.h"

static constexpr float MARTIAN_TARGET_H     = 150.0f;
static constexpr int   MARTIAN_HP           = 3;
static constexpr float MARTIAN_MOVE_SPEED   = 90.0f;
static constexpr float MARTIAN_DETECT_RANGE = 550.0f;
static constexpr float MARTIAN_ATTACK_RANGE = 320.0f;

// Energy beam: fast, 2 damage, moderate fire rate.
static constexpr float BEAM_COOLDOWN = 0.8f;
static constexpr int   BEAM_DAMAGE   = 2;
static constexpr float BEAM_SPEED    = 700.0f;

Martian::Martian(float x, float y,
                 const Level*   lvl,
                 Player*        p,
                 EntityManager* em)
    : Enemy(x, y,
            MARTIAN_TARGET_H,
            MARTIAN_HP,
            "Sprites/Enemies/Martian.png",
            sf::Color(60, 200, 120),   // alien-green placeholder
            lvl, p, em),
      hasLanded(false)
{
    moveSpeed      = MARTIAN_MOVE_SPEED;
    detectionRange = MARTIAN_DETECT_RANGE;
    attackRange    = MARTIAN_ATTACK_RANGE;

    // Start near-weightless for the flying phase.
    gravity = FLY_GRAVITY;

    // Energy beam — a fast straight projectile.
    weapon = new ProjectileWeapon(
        BEAM_COOLDOWN,
        BEAM_DAMAGE,
        -1,
        BEAM_SPEED,
        false,
        sf::Color(0, 255, 180),   // cyan-green energy beam colour
        lvl
    );

    // Begin in flying phase.
    transitionTo(new MartianFlyState(4.0f));
}

void Martian::update(float dt)
{
    // Switch to normal gravity the moment the pod first touches down.
    if (!hasLanded && onGround) {
        hasLanded = true;
        gravity   = GROUND_GRAVITY;
    }

    Enemy::update(dt);
}
