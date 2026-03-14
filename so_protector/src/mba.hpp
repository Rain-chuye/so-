#ifndef MBA_HPP
#define MBA_HPP

#include <cstdint>
#include <vector>

namespace MBA {

// Basic MBA identities for demonstration
// x ^ y = (x | y) - (x & y)
// x + y = (x ^ y) + 2 * (x & y)

inline uint8_t transform_byte(uint8_t x, uint8_t key) {
    // A more complex MBA-like transformation for a byte
    // Using a mix of bitwise and arithmetic
    uint8_t a = x;
    uint8_t b = key;

    // (a ^ b) mixed with some constants
    uint8_t res = (a | b) - (a & b);
    res = (res + 0x42) ^ 0xAA;
    res = (res | 0x1F) - (res & 0x1F);
    return res;
}

inline uint8_t inverse_transform_byte(uint8_t x, uint8_t key) {
    uint8_t b = key;
    uint8_t res = x;

    // Reverse the steps of transform_byte
    // Step 3: res = (res | 0x1F) - (res & 0x1F) is actually res ^ 0x1F
    res = (res | 0x1F) - (res & 0x1F);

    // Step 2: res = (res + 0x42) ^ 0xAA -> res = (res ^ 0xAA) - 0x42
    res = (res ^ 0xAA) - 0x42;

    // Step 1: res = (a | b) - (a & b) is res = a ^ b -> a = res ^ b
    res = (res | b) - (res & b);
    return res;
}

inline void transform_buffer(uint8_t* buffer, size_t size, uint8_t key) {
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = transform_byte(buffer[i], key ^ (i & 0xFF));
    }
}

inline void inverse_transform_buffer(uint8_t* buffer, size_t size, uint8_t key) {
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = inverse_transform_byte(buffer[i], key ^ (i & 0xFF));
    }
}

} // namespace MBA

#endif
