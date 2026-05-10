#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class EntityManager;

// Explosion — spawned by ballistic/rocket projectiles on terrain impact.
// On its first update frame it damages every entity within radius,
// then lives for a short visual lifetime before deactivating.
class Explosion : public Entity
{
private:
    float          radius;
    int            damage;
    bool           fromPlayer;
    EntityManager* entities;  // non-owning
    bool           hasExploded;
    float          lifetime;  // visual duration after blast
    sf::Texture    texture;
    sf::Sprite     sprite;

public:
    Explosion(float centerX, float centerY,
              float radius, int damage, bool fromPlayer,
              EntityManager* em);

    void update(float dt) override;
    void render(sf::RenderWindow& window, float cameraX, float cameraY) override;

    // Explosions cannot be hit by projectiles
    bool receiveProjectileHit(int, bool) override { return false; }
    void applyScreenClamp(float) override {}
};
