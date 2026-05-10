#include "ProjectileWeapon.h"
#include "StraightProjectile.h"
#include "BallisticProjectile.h"
#include "EntityManager.h"
#include "Level.h"

ProjectileWeapon::ProjectileWeapon(float     cooldown,
                                   int       damage,
                                   int       ammo,
                                   float     projectileSpeed,
                                   bool      gravityAffected,
                                   sf::Color projectileColor,
                                   const Level* lvl)
    : Weapon(cooldown),
      damage(damage),
      ammo(ammo),
      projectileSpeed(projectileSpeed),
      gravityAffected(gravityAffected),
      projectileColor(projectileColor),
      level(lvl)
{}

void ProjectileWeapon::addAmmo(int amount)
{
    if (ammo == -1) return; // infinite — nothing to add
    ammo += amount;
    if (ammo < 0) ammo = 0;
}

void ProjectileWeapon::fire(float x, float y,
                             bool  facingRight,
                             bool  fromPlayer,
                             EntityManager& em)
{
    if (!canFire() || !hasAmmo()) 
        return;

    float vx = facingRight ? projectileSpeed : -projectileSpeed;

    if (gravityAffected) {
        float vy = -projectileSpeed * 0.5f;
        em.add(new BallisticProjectile(x, y, vx, vy, damage, fromPlayer, level, &em, projectileColor));
    }
    else {
        float vy = 0.0f;
        em.add(new StraightProjectile(x, y, vx, vy, damage, fromPlayer, level, &em, projectileColor));
    }

    // Consume ammo (infinite stays at -1).
    if (ammo > 0) ammo--;

    // Restart cooldown.
    cooldownTimer = cooldown;
}
