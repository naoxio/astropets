#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <GL/glew.h>

char* load_shader(const char* path);
GLuint create_shader(const char* source, GLenum type);
GLuint create_shader_program(const char* vertexSource, const char* fragmentSource);

#endif