#include "Weapon.h"

Weapon::Weapon(float cooldown)
    : cooldown(cooldown),
      cooldownTimer(0.0f) // ready to fire immediately
{}

void Weapon::update(float dt)
{
    if (cooldownTimer > 0.0f) {
        cooldownTimer -= dt;
        if (cooldownTimer < 0.0f)
            cooldownTimer = 0.0f;
    }
}
