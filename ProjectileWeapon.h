#pragma once
#include "Weapon.h"
#include <SFML/Graphics.hpp>

class Level;

class ProjectileWeapon : public Weapon
{
private:
    int       damage;
    int       ammo;              // -1 = infinite
    float     projectileSpeed;
    bool      gravityAffected;   // false = straight, true = ballistic arc
    sf::Color projectileColor;
    float     explosionRadius;   // 0 = no explosion; >0 = area damage on impact
    const Level* level;

public:
    ProjectileWeapon(float     cooldown,
                     int       damage,
                     int       ammo,
                     float     projectileSpeed,
                     bool      gravityAffected,
                     sf::Color projectileColor,
                     const Level* lvl,
                     float     explosionRadius = 0.0f);

    int  getAmmo() const { return ammo; }
    bool hasAmmo() const { return ammo == -1 || ammo > 0; }
    void addAmmo(int amount);

    void fire(float x, float y,
              bool  facingRight,
              bool  fromPlayer,
              EntityManager& em) override;
};
