#include "Paratrooper.h"
#include "ProjectileWeapon.h"
#include "Player.h"

static const float PARA_TARGET_H = 150.0f;
static const int PARA_HP = 2;
static const float PARA_MOVE_SPEED = 110.0f;
static const float PARA_DETECT_RANGE = 450.0f;
static const float PARA_ATTACK_RANGE = 250.0f;

static const float PARACHUTE_GRAVITY = 80.0f;
static const float GROUND_GRAVITY = 1500.0f;
static const float TRIGGER_RANGE = 1600.0f; // start falling when player is this close

Paratrooper::Paratrooper(float x, float y,
                         const Level*   lvl,
                         Player*        p,
                         EntityManager* em)
    : Enemy(x, y,
            PARA_TARGET_H,
            PARA_HP,
            "Sprites/Enemies/Paratrooper.png",
            lvl, p, em),
      hasLanded(false),
      hasDeparted(false)
{
    moveSpeed      = PARA_MOVE_SPEED;
    detectionRange = PARA_DETECT_RANGE;
    attackRange    = PARA_ATTACK_RANGE;

    // Gravity is less when landing, then gets normal
    gravity = PARACHUTE_GRAVITY;

    weapon = new ProjectileWeapon(
        1.0f, 1, -1, 500.0f, false,
        sf::Color(255, 230, 50), lvl
    );

    // Loading the rebel soldier sprite ready to swap on landing
    rebelTexture.loadFromFile("Sprites/Enemies/rebel_soldier.png");

    // Keeps stuck in sky until player reaches trugger range, then starts falling
    gravity = 0.0f;
    transitionTo(new ParachuteState());
}

void Paratrooper::update(float dt)
{
    // Begin falling only once the player walks within one screen width.
    if (!hasDeparted && player) {
        float dx = player->getPosX() - positionX;
        if (dx < 0.0f) dx = -dx;
        if (dx <= TRIGGER_RANGE) {
            hasDeparted = true;
            gravity     = PARACHUTE_GRAVITY;
        }
    }

    if (!hasLanded && onGround) {
        hasLanded = true;
        gravity   = GROUND_GRAVITY;

        // Swap appearance to rebel soldier.
        if (rebelTexture.getSize().y > 0) {
            float scale = entityHeight / static_cast<float>(rebelTexture.getSize().y);
            entityWidth = static_cast<float>(rebelTexture.getSize().x) * scale;
            baseScaleX  = scale;
            baseScaleY  = scale;
            sprite.setTexture(rebelTexture, true);
            sprite.setScale(baseScaleX, baseScaleY);
        }
    }

    Enemy::update(dt);
}

int Paratrooper::getScoreValue() const
{
    return 75;
}
