#pragma once

#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#define GL_ERROR_CASE(glerror)                                                                    \
    case glerror:                                                                                 \
        snprintf(error, sizeof(error), "%s", #glerror)

inline void gl_debug(const char* file, int line)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        char error[128];

        switch (err) {
            GL_ERROR_CASE(GL_INVALID_ENUM);
            break;
            GL_ERROR_CASE(GL_INVALID_VALUE);
            break;
            GL_ERROR_CASE(GL_INVALID_OPERATION);
            break;
            GL_ERROR_CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
            break;
            GL_ERROR_CASE(GL_OUT_OF_MEMORY);
            break;
        default:
            snprintf(error, sizeof(error), "%s", "UNKNOWN_ERROR");
            break;
        }

        fprintf(stderr, "%s - %s: %d\n", error, file, line);
    }
}

#undef GL_ERROR_CASE

void error_callback(int error, const char* description);

inline void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s (%d)\n", description, error);
}

inline uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b)
{
    return static_cast<uint32_t>((r << 24) | (g << 16) | (b << 8) | 255);
}
