#pragma once

#include <GL/glew.h>
#include <string>

void validate_shader(GLuint shader, const std::string& filename);
bool validate_program(GLuint program);
