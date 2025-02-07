#ifndef LIGHT_H
#define LIGHT_H

#include "raylib.h"

typedef struct {
    Vector3 position;
    Vector3 color;
    float intensity;
} LightComponent;

// Initialize a light component
LightComponent CreateLight(Vector3 position, Vector3 color, float intensity);

// Update light properties
void UpdateLight(LightComponent* light, Vector3 position, Vector3 color, float intensity);

#endif // LIGHT_H
