#pragma once

#include <cstddef>
#include <cstdint>

#include "sprite.h"

struct Buffer {
    size_t width, height;
    uint32_t* data;
};

void buffer_clear(Buffer* buffer, uint32_t color);
void buffer_draw_sprite(Buffer* buffer, const Sprite& sprite, size_t x, size_t y, uint32_t color);
