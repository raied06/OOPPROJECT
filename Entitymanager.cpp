#include "EntityManager.h"

EntityManager::EntityManager(int initialCapacity)
    : count(0), capacity(initialCapacity)
{
    entities = new Entity * [capacity];
}

EntityManager::~EntityManager()
{
    // Delete every entity we own, then delete the array that held them.
    // Two-step. If you just do delete[] entities, the Entity objects
    // themselves leak because delete[] only frees the pointer array.
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

    // Copy pointers, not entities. The Entity objects themselves stay put,
    // we're just moving the address-book entries to a bigger book.
    for (int i = 0; i < count; i++) {
        newBuf[i] = entities[i];
    }

    delete[] entities; // free the old address book, NOT the entities
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
    // Two-pointer compaction. Single O(n) pass.
    // writeIdx lags behind readIdx whenever a dead entity is encountered.
    // Survivors get packed to the front. Dead ones get deleted in place.
    //
    // Example: [alive, dead, alive, dead, alive]
    //   read=0, write=0 -> alive, write++  -> [alive, ?, alive, ?, alive]
    //   read=1, write=1 -> dead, DELETE
    //   read=2, write=1 -> alive, entities[1] = entities[2], write++
    //   read=3, write=2 -> dead, DELETE
    //   read=4, write=2 -> alive, entities[2] = entities[4], write++
    //   count = 3 -> [alive, alive, alive]

    int writeIdx = 0;
    for (int readIdx = 0; readIdx < count; readIdx++) {
        if (entities[readIdx] == nullptr) continue;

        if (entities[readIdx]->getIsActive()) {
            entities[writeIdx++] = entities[readIdx];
        }
        else {
            delete entities[readIdx];
            entities[readIdx] = nullptr;
        }
    }
    count = writeIdx;
}