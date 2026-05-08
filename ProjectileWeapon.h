#pragma once
#include "Weapon.h"
#include <SFML/Graphics.hpp>

class Level; // forward declaration

// ═════════════════════════════════════════════════════════════════════════════
// ProjectileWeapon  —  fires StraightProjectile or BallisticProjectile
// ═════════════════════════════════════════════════════════════════════════════
// Parameterised so we can represent Pistol, HMG, Rocket Launcher, etc.
// without a separate class per weapon — they differ only in construction args.
//
//   gravityAffected = false → StraightProjectile (bullets, laser-style)
//   gravityAffected = true  → BallisticProjectile (grenades, rockets)
//
// ammo == -1 means infinite ammo (used for Pistol, default weapons).
// ─────────────────────────────────────────────────────────────────────────────
class ProjectileWeapon : public Weapon
{
private:
    int       damage;
    int       ammo;              // -1 = infinite
    float     projectileSpeed;   // pixels / second
    bool      gravityAffected;   // false → straight, true → ballistic arc
    sf::Color projectileColor;
    const Level* level;          // non-owning; forwarded to each Projectile ctor

public:
    // ammo = -1 for infinite
    ProjectileWeapon(float     cooldown,
                     int       damage,
                     int       ammo,
                     float     projectileSpeed,
                     bool      gravityAffected,
                     sf::Color projectileColor,
                     const Level* lvl);

    // ── Ammo management ──────────────────────────────────────────────────────
    int  getAmmo() const { return ammo; }
    bool hasAmmo() const { return ammo == -1 || ammo > 0; }
    void addAmmo(int amount);

    // ── Weapon interface ─────────────────────────────────────────────────────
    void fire(float x, float y,
              bool  facingRight,
              bool  fromPlayer,
              EntityManager& em) override;
};
