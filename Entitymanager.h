#pragma once
#include "Entity.h"

class Level;

// Owns every Entity* added to it. Caller must NOT delete after calling add().
// Uses a raw growable array - no std::vector allowed.

class EntityManager
{
private:
    Entity** entities; // AGGREGATION
    int count; // counter for how many entities present in the array
    int capacity;

    // Doubles the internal buffer. Called automatically by add() when full.
    void grow();

    // copying is disabled because it would mean two managers and both trying to delete the same Entity pointers. First
    // one will execute and second delete would cause instant crash bcz trying to delete something that doesn't exist.
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

public:
    EntityManager(int initialCapacity = 16);
    ~EntityManager();

    void add(Entity* e);

    // Calls update(dt) on every active entity.
    void updateAll(float dt);

    // Calls render on every active entity
    void renderAll(sf::RenderWindow& window, float cameraX, float cameraY);

    // Deletes entities where isActive == false, compacts the array.
    // Called once per frame in update function of PlayState.
    void removeDead();

    int getCount() const;
    Entity* getEntity(int i) const;
};
