#include "EntityManager.h"

EntityManager::EntityManager(int initialCapacity)
    : count(0), capacity(initialCapacity)
{
    entities = new Entity * [capacity];
}

EntityManager::~EntityManager()
{
    for (int i = 0; i < count; i++) {
        delete entities[i];
        entities[i] = nullptr;
    }
    delete[] entities;
    entities = nullptr;
}

void EntityManager::grow()
{
    int newCap = (capacity == 0) ? 16 : capacity * 2;
    Entity** newBuf = new Entity * [newCap];

    for (int i = 0; i < count; i++) {
        newBuf[i] = entities[i];
    }

    delete[] entities;
    entities = newBuf;
    capacity = newCap;
}

void EntityManager::add(Entity* e)
{
    if (e == nullptr) return;

    if (count >= capacity) {
        grow();
    }
    entities[count++] = e;
}

void EntityManager::updateAll(float dt)
{
    for (int i = 0; i < count; i++) {
        if (entities[i] && entities[i]->getIsActive()) {
            entities[i]->update(dt);
        }
    }
}

void EntityManager::renderAll(sf::RenderWindow& window, float cameraX, float cameraY)
{
    for (int i = 0; i < count; i++) {
        if (entities[i] && entities[i]->getIsActive()) {
            entities[i]->render(window, cameraX, cameraY);
        }
    }
}

void EntityManager::removeDead()
{
    // Removing dead entities from the array and updating the indexes of alive entities
    int alive = 0;
    for (int i = 0; i < count; i++) {
        if (entities[i] == nullptr) continue;

        if (entities[i]->getIsActive()) {
            entities[alive++] = entities[i];
        }
        else {
            delete entities[i];
            entities[i] = nullptr;
        }
    }
    count = alive;
}

int EntityManager::getCount() const
{
    return count;
}

Entity* EntityManager::getEntity(int i) const
{
    return (i >= 0 && i < count) ? entities[i] : nullptr;
}
