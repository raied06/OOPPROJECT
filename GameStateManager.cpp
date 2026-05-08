#include "GameStateManager.h"

GameStateManager::GameStateManager(int cap): capacity(cap), topIndex(-1), states(new GameState* [capacity]) 
{
    // topIndex is -1 bcz the indexes of arrays start from index 0, and if we give the top index a value of 0,
    // it would mean that there would be a state at index 0, which we actually haven't given yet,
    // so we give a value of -1 and whenever a state is inserted, this value is incremented.
}

GameStateManager::~GameStateManager()
{
    //Deleting the state objects at indexes
    for (int i = 0; i <= topIndex; i++) {
        delete states[i];
    }
    // deleting the array of states pointer
    delete[] states;
}

void GameStateManager::pushState(GameState* state)
{
    // checking if the array (stack) is full or not, if full then doubling its size first and then pushing the state
    if (topIndex >= this->capacity - 1) {
        this->capacity *= 2;
        GameState** temp = new GameState * [capacity];
        for (int i = 0; i <= topIndex; i++) {
            temp[i] = states[i];
        }
        delete[] states; // Delete old array
        states = temp;   // Point to new array
    }

    // Pushing the new state
    states[++topIndex] = state;
}

void GameStateManager::popState()
{
    // deletes the object at the top index and top index is decremented so that the previous state start to run
    if (topIndex >= 0) {
        delete states[topIndex];
        states[topIndex] = nullptr;
        topIndex--;
    }
}

GameState* GameStateManager::getCurrentState()
{
    if (topIndex >= 0) {
        return states[topIndex];
    }
    return nullptr;
}
