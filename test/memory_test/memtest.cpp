#include <iostream>
#include <cstdint>
#include <cstddef>

extern "C" {

// This function accepts a raw memory buffer from Node.js
void handle_buffer(uint8_t* data, size_t length) {
    std::cout << "[C++] Received buffer of length: " << length << std::endl;
    std::cout << "[C++] Data: ";
    for (size_t i = 0; i < length; ++i) {
        std::cout << std::hex << (int)data[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

}
