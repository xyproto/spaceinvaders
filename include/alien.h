#pragma once

struct Alien {
    size_t x, y;
    uint8_t type;
};

enum AlienType : uint8_t { ALIEN_DEAD = 0, ALIEN_TYPE_A = 1, ALIEN_TYPE_B = 2, ALIEN_TYPE_C = 3 };
