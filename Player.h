#pragma once
#include "Soldier.h"

class EntityManager; // forward declaration — full include in Player.cpp
class Weapon;        // forward declaration

// Player class has some extra specialized features that in soldier class
// so it only handles inputs for the player and sets its sprite, rest of the
// mechanics are calculated inside Soldier class

class Player : public Soldier
{
private:
    bool jumpHeldLastFrame; // This is to prevent infinite jumping logic if jump key is held continuosly
    bool fireHeldLastFrame; // Prevents holding mouse = bullet stream (one shot per click)

    Weapon*        weapon;   // OWNED — starts as Pistol, swappable on pickup
    EntityManager* entities; // non-owning — only used to spawn projectiles

public:
// CONSTRUCTOR
    // hp: starting health  |  em: shared entity pool (non-owning, must outlive Player)
    Player(float x, float y, const Level* lvl, EntityManager* em, int hp = 5);
// DESTRUCTOR
    virtual ~Player();

    // Non-copyable (owns weapon pointer)
    Player(const Player&)            = delete;
    Player& operator=(const Player&) = delete;

// FUNCTIONS
    void handleInput();
    virtual void update(float dt) override; // It calls soldier update inside it

    // Swap weapon on pickup — Player takes ownership of newWeapon
    void    equipWeapon(Weapon* newWeapon);
    Weapon* getWeapon() const { return weapon; }

    virtual void applyScreenClamp(float cameraX) override;
};
