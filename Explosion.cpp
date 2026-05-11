#include "Explosion.h"
#include "EntityManager.h"

static constexpr float VISUAL_LIFETIME = 0.25f;

Explosion::Explosion(float centerX, float centerY,
                     float radius, int damage, bool fromPlayer,
                     EntityManager* em)
    : Entity(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f),
      radius(radius),
      damage(damage),
      fromPlayer(fromPlayer),
      entities(em),
      hasExploded(false),
      lifetime(VISUAL_LIFETIME)
{
    if (!texture.loadFromFile("Sprites/blast.png"))
        texture.loadFromFile("Sprites/blast.png"); // silent fallback — sprite stays blank

    sprite.setTexture(texture, true);

    // Scale so the sprite exactly fills the blast diameter on both axes.
    float sx = (radius * 2.0f) / static_cast<float>(texture.getSize().x);
    float sy = (radius * 2.0f) / static_cast<float>(texture.getSize().y);
    sprite.setScale(sx, sy);
    sprite.setColor(sf::Color(255, 255, 255, 255));
}

void Explosion::update(float dt)
{
    if (!isActive) return;

    if (!hasExploded) {
        float cx = positionX + radius;
        float cy = positionY + radius;

        int n = entities->getCount();
        for (int i = 0; i < n; i++) {
            Entity* e = entities->getEntity(i);
            if (!e || !e->getIsActive() || e == this) continue;

            float ex = e->getPosX() + e->getEntityWidth()  * 0.5f;
            float ey = e->getPosY() + e->getEntityHeight() * 0.5f;
            float dx = ex - cx;
            float dy = ey - cy;
            // Avoid sqrt — compare squared distances
            if (dx * dx + dy * dy < radius * radius)
                e->receiveExplosionHit(damage, fromPlayer);
        }
        hasExploded = true;
    }

    lifetime -= dt;
    if (lifetime <= 0.0f) {
        deactivateEntity();
        return;
    }

    // Fade alpha from 255 → 0 as lifetime runs out.
    float t     = lifetime / VISUAL_LIFETIME;
    sf::Uint8 a = static_cast<sf::Uint8>(255.0f * t);
    sprite.setColor(sf::Color(255, 255, 255, a));
}

void Explosion::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive || !hasExploded) return;
    // positionX/Y is already the top-left of the blast square (center - radius).
    sprite.setPosition(positionX - cameraX, positionY - cameraY);
    window.draw(sprite);
}
