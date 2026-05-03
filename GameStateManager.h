#pragma once
#include "GameState.h"

class GameStateManager {
private:
    GameState** states; //pointer to an array of gamestate pointers
    int capacity;
    int topIndex;

public:
    GameStateManager(int cap = 5) {
        capacity = cap;
        topIndex = -1;
        states = new GameState * [capacity]; //allocate the array of pointers
    }

    ~GameStateManager() {
        // 1. Delete all the actual state objects we allocated
        for (int i = 0; i <= topIndex; i++) {
            delete states[i];
        }
        // 2. Delete the array holding the pointers
        delete[] states;
    }

    void pushState(GameState* state) {
        // If the stack is full, we have to manually resize the array
        if (topIndex >= capacity - 1) {
            capacity *= 2;
            GameState** temp = new GameState * [capacity];
            for (int i = 0; i <= topIndex; i++) {
                temp[i] = states[i];
            }
            delete[] states; // Delete old array
            states = temp;   // Point to new array
        }

        // Push the new state
        states[++topIndex] = state;
    }

    void popState() {
        if (topIndex >= 0) {
            delete states[topIndex]; // Delete the actual object
            states[topIndex] = nullptr; // Avoid dangling pointers
            topIndex--;
        }
    }

    GameState* getCurrentState() {
        if (topIndex >= 0) {
            return states[topIndex];
        }
        return nullptr;
    }
};