#pragma once
#include "Soldier.h"

// Player class has some extra specialized features that in soldier class
// so it only handles inputs for the player and sets its sprite, rest of the
// mechanics are calculated inside Soldier class

class Player : public Soldier
{
private:
    bool jumpHeldLastFrame; // This is to prevent infinite jumping logic if jump key is held continuosly
public:
// CONSTRUCTOR
    Player(float x, float y, const Level* lvl);
// DESTRUCTOR
    virtual ~Player();
// FUNCTIONS
    void handleInput();
    virtual void update(float dt) override; // It calls soldier update inside it
};