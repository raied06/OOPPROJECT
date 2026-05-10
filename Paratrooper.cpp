#include "Paratrooper.h"
#include "ProjectileWeapon.h"

static constexpr float PARA_TARGET_H     = 150.0f;
static constexpr int   PARA_HP           = 2;
static constexpr float PARA_MOVE_SPEED   = 110.0f;
static constexpr float PARA_DETECT_RANGE = 450.0f;
static constexpr float PARA_ATTACK_RANGE = 250.0f;

Paratrooper::Paratrooper(float x, float y,
                         const Level*   lvl,
                         Player*        p,
                         EntityManager* em)
    : Enemy(x, y,
            PARA_TARGET_H,
            PARA_HP,
            "Sprites/Enemies/Paratrooper.png",
            lvl, p, em),
      hasLanded(false)
{
    moveSpeed      = PARA_MOVE_SPEED;
    detectionRange = PARA_DETECT_RANGE;
    attackRange    = PARA_ATTACK_RANGE;

    // Start with parachute gravity — overridden on first landing.
    gravity = PARACHUTE_GRAVITY;

    weapon = new ProjectileWeapon(
        1.0f,                       // cooldown
        1,                          // damage
        -1,                         // infinite ammo
        500.0f,                     // speed
        false,                      // straight shot
        sf::Color(255, 230, 50),    // yellow bullet
        lvl
    );

    // Begin in parachute descent — overrides default PatrolState.
    transitionTo(new ParachuteState());
}

void Paratrooper::update(float dt)
{
    // Switch to normal gravity the first time we land.
    if (!hasLanded && onGround) {
        hasLanded = true;
        gravity   = GROUND_GRAVITY;
    }

    Enemy::update(dt);
}
