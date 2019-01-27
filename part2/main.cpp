/*
 * From "Space Invaders from Scratch"
 * http://nicktasios.nl/posts/space-invaders-from-scratch-part-2.html
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "buffer.h"
#include "shaders.h"
#include "spaceinvaders.h"

int main()
{
    const size_t buffer_width = 224;
    const size_t buffer_height = 256;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window
        = glfwCreateWindow(buffer_width, buffer_height, "Space Invaders", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error initializing GLEW.\n");
        glfwTerminate();
        return -1;
    }
    int glVersion[2] = { -1, 1 };
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

    gl_debug(__FILE__, __LINE__);

    printf("Using OpenGL: %d.%d\n", glVersion[0], glVersion[1]);
    printf("Renderer used: %s\n", glGetString(GL_RENDERER));
    printf("Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glClearColor(1.0, 0.0, 0.0, 1.0);

    // Create graphics buffer
    Buffer buffer;
    buffer.width = buffer_width;
    buffer.height = buffer_height;
    buffer.data = new uint32_t[buffer.width * buffer.height];

    buffer_clear(&buffer, 0);

    // Create texture for presenting buffer to OpenGL
    GLuint buffer_texture;
    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, static_cast<GLsizei>(buffer.width),
        static_cast<GLsizei>(buffer.height), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create vao for generating fullscreen triangle
    GLuint fullscreen_triangle_vao;
    glGenVertexArrays(1, &fullscreen_triangle_vao);

    // Create shader for displaying buffer
    static const char* fragment_shader = R"(
                                         #version 330

                                         uniform sampler2D buffer;
                                         noperspective in vec2 TexCoord;

                                         out vec3 outColor;

                                         void main(void) {
                                             outColor = texture(buffer, TexCoord).rgb;
                                         }
                                         )";

    static const char* vertex_shader = R"(
                                       #version 330

                                       noperspective out vec2 TexCoord;

                                       void main(void) {

                                           TexCoord.x = (gl_VertexID == 2)? 2.0: 0.0;
                                           TexCoord.y = (gl_VertexID == 1)? 2.0: 0.0;

                                           gl_Position = vec4(2.0 * TexCoord - 1.0, 0.0, 1.0);
                                       }
                                       )";

    GLuint shader_id = glCreateProgram();

    {
        // Create vertex shader
        GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shader_vp, 1, &vertex_shader, nullptr);
        glCompileShader(shader_vp);
        validate_shader(shader_vp, vertex_shader);
        glAttachShader(shader_id, shader_vp);

        glDeleteShader(shader_vp);
    }

    {
        // Create fragment shader
        GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shader_fp, 1, &fragment_shader, nullptr);
        glCompileShader(shader_fp);
        validate_shader(shader_fp, fragment_shader);
        glAttachShader(shader_id, shader_fp);

        glDeleteShader(shader_fp);
    }

    glLinkProgram(shader_id);

    if (!validate_program(shader_id)) {
        fprintf(stderr, "Error while validating shader.\n");
        glfwTerminate();
        glDeleteVertexArrays(1, &fullscreen_triangle_vao);
        delete[] buffer.data;
        return -1;
    }

    glUseProgram(shader_id);

    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);

    // OpenGL setup
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(fullscreen_triangle_vao);

    // Prepare game
    Sprite alien_sprite;
    alien_sprite.width = 11;
    alien_sprite.height = 8;
    alien_sprite.data = new uint8_t[88] {
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, // ..@.....@..
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, // ...@...@...
        0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, // ..@@@@@@@..
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, // .@@.@@@.@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, // @.@@@@@@@.@
        1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, // @.@.....@.@
        0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0 // ...@@.@@...
    };

    uint32_t clear_color = rgb_to_uint32(0, 128, 0);

    while (!glfwWindowShouldClose(window)) {
        buffer_clear(&buffer, clear_color);

        buffer_draw_sprite(&buffer, alien_sprite, 112, 128, rgb_to_uint32(128, 0, 0));

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(buffer.width),
            static_cast<GLsizei>(buffer.height), GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    glDeleteVertexArrays(1, &fullscreen_triangle_vao);

    delete[] alien_sprite.data;
    delete[] buffer.data;

    return 0;
}
