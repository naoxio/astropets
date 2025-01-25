
#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <GL/glew.h>
#include <stdio.h>

typedef struct {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    int indexCount;
    float scale;
} Model;

Model load_model(const char* path, float scale);
void draw_model(Model model);

#endif
