#pragma once
#include "Soldier.h"

// Player just translates keyboard state into Soldier intents.
// All physics/collision lives in Soldier. Keep this file dumb.

class Player : public Soldier
{
private:
    bool jumpHeldLastFrame;  // edge detection: prevents holding Space = infinite bunny hop

public:
    Player(float x, float y, const Level* lvl);
    virtual ~Player();

    // Call this BEFORE update() in the game loop so intent is set first.
    void handleInput();

    // Override update to slot handleInput in automatically,
    // so PlayState doesn't have to call two separate functions.
    virtual void update(float dt) override;
};