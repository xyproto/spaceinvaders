#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

struct Sprite {
    size_t width, height;
    uint8_t* data;
};

inline bool sprite_overlap_check(
    const Sprite& sp_a, size_t x_a, size_t y_a, const Sprite& sp_b, size_t x_b, size_t y_b)
{
    // NOTE: For simplicity we just check for overlap of the sprite
    // rectangles. Instead, if the rectangles overlap, we should
    // further check if any pixel of sprite A overlap with any of
    // sprite B.
    if (x_a < x_b + sp_b.width && x_a + sp_a.width > x_b && y_a < y_b + sp_b.height
        && y_a + sp_a.height > y_b) {
        return true;
    }

    return false;
}
