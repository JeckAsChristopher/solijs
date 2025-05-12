#include <iostream>
#include <cstdint>
#include <cstddef>

extern "C" {

// This is the game tick function that will be called from Node.js
void game_tick(float deltaTime) {
    std::cout << "[C++] Game tick called with deltaTime: " << deltaTime << " seconds" << std::endl;
}

}
