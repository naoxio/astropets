#ifndef TERRARIUM_H
#define TERRARIUM_H

#include <raylib.h>
#include <raymath.h>
#include "light.h"
typedef struct {
    Model sphere;
    float radius;
    Vector3 position;
    Shader shader;
} GlassSphere;

typedef struct {
    Model surface;
    float height;  
    Shader shader;
} Ground;

typedef struct {
    GlassSphere glass;
    Ground ground;
    LightComponent internalLight;
} TerrariumSystem;

TerrariumSystem InitializeTerrariumSystem(Shader glassShader, Shader groundShader);
void DrawTerrariumSystem(TerrariumSystem* terrarium, Camera3D camera);
void UnloadTerrariumSystem(TerrariumSystem* terrarium);

#endif // TERRARIUM_H