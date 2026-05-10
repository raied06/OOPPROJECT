#pragma once
#include "Projectile.h"
#include <SFML/Graphics.hpp>

class BallisticProjectile : public Projectile
{
private:
    float gravity;
    float maxFallSpeed;
    float explosionRadius; // 0 = no explosion; >0 = spawn Explosion on impact

    sf::CircleShape shape;

public:
    BallisticProjectile(float x, float y,
                        float vx, float vy,
                        int   damage,
                        bool  fromPlayer,
                        const Level* lvl,
                        EntityManager* em,
                        sf::Color color          = sf::Color(210, 105, 30),
                        float     explosionRadius = 0.0f);

    void update(float dt) override;
    void render(sf::RenderWindow& window, float cameraX, float cameraY) override;
};
