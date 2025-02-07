#include "light.h"

LightComponent CreateLight(Vector3 position, Vector3 color, float intensity) {
    LightComponent light = {0};
    light.position = position;
    light.color = color;
    light.intensity = intensity;
    return light;
}

void UpdateLight(LightComponent* light, Vector3 position, Vector3 color, float intensity) {
    light->position = position;
    light->color = color;
    light->intensity = intensity;
}
