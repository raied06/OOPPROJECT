#pragma once

class EntityManager; // forward declaration — full include not needed in header

// ═════════════════════════════════════════════════════════════════════════════
// Weapon  —  abstract base for all weapons (per UML)
// ═════════════════════════════════════════════════════════════════════════════
// Handles cooldown bookkeeping. Subclasses decide what projectile to create.
//
// Usage pattern:
//   1. Call weapon->update(dt) each frame to tick the cooldown.
//   2. Call weapon->fire(...) when the holder wants to shoot;
//      the weapon checks canFire() internally and does nothing if on cooldown.
// ─────────────────────────────────────────────────────────────────────────────
class Weapon
{
protected:
    float cooldown;      // minimum seconds between shots
    float cooldownTimer; // counts DOWN to 0; fire is allowed when <= 0

public:
    explicit Weapon(float cooldown);
    virtual ~Weapon() {}

    // Non-copyable (subclasses own heap resources)
    Weapon(const Weapon&)            = delete;
    Weapon& operator=(const Weapon&) = delete;

    // Tick the cooldown each frame.
    void update(float dt);

    // True when the weapon is ready to fire.
    bool canFire() const { return cooldownTimer <= 0.0f; }

    // Spawn a projectile into the entity pool.
    //   x, y        — world-space spawn point (caller's centre, typically)
    //   facingRight — direction of fire
    //   fromPlayer  — true for player bullets, false for enemy bullets
    //                 (used by collision system to avoid friendly fire)
    //   em          — entity pool that takes ownership of the new projectile
    virtual void fire(float x, float y,
                      bool  facingRight,
                      bool  fromPlayer,
                      EntityManager& em) = 0;
};
