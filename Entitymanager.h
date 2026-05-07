#pragma once
#include "Entity.h"

class Level;

// Owns every Entity* added to it. Caller must NOT delete after calling add().
// Uses a raw growable array - no std::vector allowed.

class EntityManager
{
private:
    Entity** entities;
    int      count;
    int      capacity;

    // Doubles the internal buffer. Called automatically by add() when full.
    void grow();

    // Disabled: copying would mean two managers both trying to delete
    // the same Entity pointers. Second delete = instant crash.
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

public:
    EntityManager(int initialCapacity = 16);
    ~EntityManager();

    // Takes ownership. Do NOT delete the pointer after calling this.
    void add(Entity* e);

    // Calls update(dt) on every active entity.
    void updateAll(float dt);

    // Calls render on every active entity.
    void renderAll(sf::RenderWindow& window, float cameraX, float cameraY);

    // Deletes entities where isActive == false, compacts the array.
    // Call once per frame at the END of update, never mid-iteration.
    void removeDead();

    int     getCount()  const { return count; }
    Entity* get(int i)  const { return (i >= 0 && i < count) ? entities[i] : nullptr; }
};
