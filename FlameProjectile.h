#pragma once
#include "Projectile.h"
#include <SFML/Graphics.hpp>

// FlameProjectile — short-range fire pellet that burns out after maxRange px.
class FlameProjectile : public Projectile
{
private:
    float maxRange;
    float distanceTraveled;
    sf::RectangleShape shape;

public:
    FlameProjectile(float x, float y, float vx,
                    int   damage, bool fromPlayer,
                    const Level* lvl, EntityManager* em,
                    float maxRange = 180.0f);

    void update(float dt) override;
    void render(sf::RenderWindow& window, float cameraX, float cameraY) override;

protected:
    // Fire deals damage through the fire channel (mummies only die from this).
    bool dispatchHit(Entity* target) override
    {
        return target->receiveFireHit(damage, fromPlayer);
    }
};
