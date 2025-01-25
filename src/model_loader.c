
#include "model_loader.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include <stdlib.h>
#include <string.h>

Model load_model(const char* path, float scale) {
   Model model = {0};  // Declare and initialize the Model struct
   model.scale = scale;  // Store the scale
   cgltf_options options = {0};
   cgltf_data* data = NULL;
   
   if (cgltf_parse_file(&options, path, &data) != cgltf_result_success) {
       printf("Failed to parse glTF file\n");
       return model;
   }

   if (cgltf_load_buffers(&options, data, path) != cgltf_result_success) {
       printf("Failed to load glTF buffers\n");
       cgltf_free(data);
       return model;
   }

   cgltf_mesh* mesh = &data->meshes[0];
   cgltf_primitive* prim = &mesh->primitives[0];

   // Get vertex positions
   cgltf_accessor* pos_accessor = prim->attributes[0].data;
   float* positions = malloc(pos_accessor->count * 3 * sizeof(float));
   
   for (size_t i = 0; i < pos_accessor->count; i++) {
       cgltf_accessor_read_float(pos_accessor, i, &positions[i * 3], 3);
       // Use the scale parameter
       positions[i * 3] *= scale;     // Scale X
       positions[i * 3 + 1] *= scale; // Scale Y
       positions[i * 3 + 2] *= scale; // Scale Z
   }

   // Get indices
   cgltf_accessor* idx_accessor = prim->indices;
   unsigned int* indices = malloc(idx_accessor->count * sizeof(unsigned int));
   
   for (size_t i = 0; i < idx_accessor->count; i++) {
       indices[i] = (unsigned int)cgltf_accessor_read_index(idx_accessor, i);
   }

   // Create buffers
   glGenVertexArrays(1, &model.vao);
   glGenBuffers(1, &model.vbo);
   glGenBuffers(1, &model.ebo);

   glBindVertexArray(model.vao);

   glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
   glBufferData(GL_ARRAY_BUFFER, pos_accessor->count * 3 * sizeof(float), positions, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_accessor->count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   model.indexCount = idx_accessor->count;

   free(positions);
   free(indices);
   cgltf_free(data);
   
   return model;
}

void draw_model(Model model) {
   glBindVertexArray(model.vao);
   glDrawElements(GL_TRIANGLES, model.indexCount, GL_UNSIGNED_INT, 0);
}