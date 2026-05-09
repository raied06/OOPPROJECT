#include "StraightProjectile.h"
#include "EntityManager.h"

static constexpr float BULLET_W = 14.0f;
static constexpr float BULLET_H =  4.0f;

StraightProjectile::StraightProjectile(float x, float y,
                                        float vx, float vy,
                                        int   damage,
                                        bool  fromPlayer,
                                        const Level* lvl,
                                        EntityManager* em,
                                        sf::Color color)
    : Projectile(x - BULLET_W * 0.5f,
                 y - BULLET_H * 0.5f,
                 BULLET_W, BULLET_H,
                 damage, fromPlayer, lvl, em)
{
    velocityX = vx;
    velocityY = vy;

    shape.setSize(sf::Vector2f(BULLET_W, BULLET_H));
    shape.setFillColor(color);
}

void StraightProjectile::update(float dt)
{
    if (!isActive) return;

    positionX += velocityX * dt;
    positionY += velocityY * dt;

    // ── World-bounds cull ────────────────────────────────────────────────────
    float worldW = static_cast<float>(level->getWidthInPixels());
    float worldH = static_cast<float>(level->getHeightInPixels());

    if (positionX + BULLET_W < 0.0f || positionX > worldW ||
        positionY + BULLET_H < 0.0f || positionY > worldH) {
        deactivateEntity();
        return;
    }

    // ── Tile collision ───────────────────────────────────────────────────────
    float checkX = (velocityX >= 0.0f) ? positionX + BULLET_W : positionX;
    float checkY = positionY + BULLET_H * 0.5f;

    if (level->isSolidAtPixel(checkX, checkY)) {
        deactivateEntity();
        return;
    }

    // ── Entity collision (pure virtual dispatch — no dynamic_cast) ───────────
    checkEntityCollisions();
}

void StraightProjectile::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;
    shape.setPosition(positionX - cameraX, positionY - cameraY);
    window.draw(shape);
}
