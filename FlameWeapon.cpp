#include "FlameWeapon.h"
#include "FlameProjectile.h"
#include "EntityManager.h"

FlameWeapon::FlameWeapon(float cooldown, int damage, const Level* lvl, float maxRange)
    : Weapon(cooldown), damage(damage), maxRange(maxRange), level(lvl)
{}

void FlameWeapon::fire(float x, float y, bool facingRight, bool fromPlayer,
                        EntityManager& em)
{
    if (!canFire()) return;
    cooldownTimer = cooldown;

    float speed = facingRight ? 500.0f : -500.0f;
    em.add(new FlameProjectile(x, y, speed, damage, fromPlayer, level, &em, maxRange));
}
