#include "BallisticProjectile.h"
#include "EntityManager.h"

static constexpr float RADIUS = 7.0f;

BallisticProjectile::BallisticProjectile(float x, float y,
                                          float vx, float vy,
                                          int   damage,
                                          bool  fromPlayer,
                                          const Level* lvl,
                                          EntityManager* em,
                                          sf::Color color)
    : Projectile(x - RADIUS,
                 y - RADIUS,
                 RADIUS * 2.0f, RADIUS * 2.0f,
                 damage, fromPlayer, lvl, em),
      gravity(900.0f),
      maxFallSpeed(1000.0f)
{
    velocityX = vx;
    velocityY = vy;

    shape.setRadius(RADIUS);
    shape.setFillColor(color);
    shape.setOutlineColor(sf::Color(80, 40, 10));
    shape.setOutlineThickness(1.5f);
}

void BallisticProjectile::update(float dt)
{
    if (!isActive) return;

    velocityY += gravity * dt;
    if (velocityY > maxFallSpeed) velocityY = maxFallSpeed;

    positionX += velocityX * dt;
    positionY += velocityY * dt;

    // ── World-bounds cull ────────────────────────────────────────────────────
    if (positionX < 0.0f ||
        positionX > static_cast<float>(level->getWidthInPixels()) ||
        positionY > static_cast<float>(level->getHeightInPixels())) {
        deactivateEntity();
        return;
    }

    // ── Entity collision (pure virtual dispatch — no dynamic_cast) ───────────
    checkEntityCollisions();
    if (!isActive) return; // hit something — skip tile check

    // ── Ground / wall contact → deactivate ──────────────────────────────────
    float centerX = positionX + RADIUS;
    float centerY = positionY + RADIUS;
    float bottomY = positionY + RADIUS * 2.0f;

    if (level->isSolidAtPixel(centerX, bottomY) ||
        level->isSolidAtPixel(positionX,              centerY) ||
        level->isSolidAtPixel(positionX + RADIUS * 2.0f, centerY)) {
        deactivateEntity();
    }
}

void BallisticProjectile::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;
    shape.setPosition(positionX - cameraX, positionY - cameraY);
    window.draw(shape);
}
