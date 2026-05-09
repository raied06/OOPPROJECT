#pragma once
#include "Projectile.h"
#include <SFML/Graphics.hpp>

// ═════════════════════════════════════════════════════════════════════════════
// StraightProjectile  —  travels in a straight line at constant velocity
// ═════════════════════════════════════════════════════════════════════════════
// Used for: Pistol, HMG, Laser (speed difference only).
// Deactivates when it hits a solid tile or flies out of the world bounds.
//
// Rendered as a coloured rectangle — swap in a sprite texture later.
// ─────────────────────────────────────────────────────────────────────────────
class StraightProjectile : public Projectile
{
private:
    sf::RectangleShape shape; // visual (no texture needed yet)

public:
    // vx, vy : initial velocity components (pixels/sec).
    //          Typically vy=0 for horizontal fire; non-zero for angled shots.
    StraightProjectile(float x, float y,
                       float vx, float vy,
                       int   damage,
                       bool  fromPlayer,
                       const Level* lvl,
                       EntityManager* em,
                       sf::Color color = sf::Color::Yellow);

    void update(float dt) override;
    void render(sf::RenderWindow& window, float cameraX, float cameraY) override;
};
