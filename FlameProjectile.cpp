#include "FlameProjectile.h"
#include "EntityManager.h"

static constexpr float FW = 18.0f;
static constexpr float FH =  9.0f;

FlameProjectile::FlameProjectile(float x, float y, float vx,
                                  int   damage, bool fromPlayer,
                                  const Level* lvl, EntityManager* em,
                                  float maxRange)
    : Projectile(x, y, FW, FH, damage, fromPlayer, lvl, em),
      maxRange(maxRange),
      distanceTraveled(0.0f)
{
    velocityX = vx;
    velocityY = 0.0f;
    shape.setSize(sf::Vector2f(FW, FH));
    shape.setFillColor(sf::Color(255, 80, 0));   // deep orange
}

void FlameProjectile::update(float dt)
{
    if (!isActive) return;

    float dx = velocityX * dt;
    positionX += dx;
    distanceTraveled += (dx < 0.0f ? -dx : dx);

    // Fade colour as it travels: orange → yellow → transparent
    float t = 1.0f - distanceTraveled / maxRange;
    if (t < 0.0f) t = 0.0f;
    sf::Uint8 alpha = static_cast<sf::Uint8>(200.0f * t);
    shape.setFillColor(sf::Color(255, static_cast<sf::Uint8>(80 + 175 * (1.0f - t)), 0, alpha));

    if (distanceTraveled >= maxRange) { deactivateEntity(); return; }

    float worldW = static_cast<float>(level->getWidthInPixels());
    if (positionX < 0.0f || positionX > worldW) { deactivateEntity(); return; }

    if (level->isSolidAtPixel(positionX + FW * 0.5f, positionY + FH * 0.5f)) {
        deactivateEntity(); return;
    }

    checkEntityCollisions();
}

void FlameProjectile::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;
    shape.setPosition(positionX - cameraX, positionY - cameraY);
    window.draw(shape);
}
