#include "LaserBeam.h"

static constexpr float BEAM_H    = 4.0f;
static constexpr float MAX_LIFE  = 0.12f;

LaserBeam::LaserBeam(float x, float y, float length, bool facingRight)
    : Entity(facingRight ? x : x - length,
             y - BEAM_H * 0.5f,
             length, BEAM_H),
      length(length),
      lifetime(MAX_LIFE)
{
    shape.setSize(sf::Vector2f(length, BEAM_H));
    shape.setFillColor(sf::Color(0, 255, 200, 220)); // cyan-green
}

void LaserBeam::update(float dt)
{
    if (!isActive) return;
    lifetime -= dt;

    // Fade out
    float t = lifetime / MAX_LIFE;
    sf::Uint8 alpha = static_cast<sf::Uint8>(220.0f * t);
    shape.setFillColor(sf::Color(0, 255, 200, alpha));

    if (lifetime <= 0.0f) deactivateEntity();
}

void LaserBeam::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;
    shape.setPosition(positionX - cameraX, positionY - cameraY);
    window.draw(shape);
}
