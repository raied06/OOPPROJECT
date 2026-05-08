#pragma once
#include "GameState.h"

class GameStateManager {
private:
    int capacity;
    GameState** states; //pointer to an array of gamestate pointers
    int topIndex;

public:
// CONSTRUCTOR
    GameStateManager(int cap = 5);
// DESTRUCTOR
    ~GameStateManager();
// FUNCTIONS
    void pushState(GameState* state);
    void popState();
    GameState* getCurrentState();
};