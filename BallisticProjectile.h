#pragma once
#include "Projectile.h"
#include <SFML/Graphics.hpp>

// ═════════════════════════════════════════════════════════════════════════════
// BallisticProjectile  —  parabolic arc projectile (grenades, rockets)
// ═════════════════════════════════════════════════════════════════════════════
// Subject to its own gravity constant (lighter than player gravity so arcs
// are easier to aim).  Deactivates — dealing area damage — on ground contact.
//
// Rendered as a coloured circle. Swap in a sprite later.
// ─────────────────────────────────────────────────────────────────────────────
class BallisticProjectile : public Projectile
{
private:
    float gravity;
    float maxFallSpeed;

    sf::CircleShape shape;

public:
    // vx, vy : initial velocity (positive vy = downward, negative = upward arc)
    BallisticProjectile(float x, float y,
                        float vx, float vy,
                        int   damage,
                        bool  fromPlayer,
                        const Level* lvl,
                        EntityManager* em,
                        sf::Color color = sf::Color(210, 105, 30)); // chocolate-brown

    void update(float dt) override;
    void render(sf::RenderWindow& window, float cameraX, float cameraY) override;
};
