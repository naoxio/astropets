#ifndef TERRARIUM_H
#define TERRARIUM_H

#include <raylib.h>
#include <raymath.h>

typedef struct {
    Model sphere;
    float radius;
    Vector3 position;
    Shader shader;
} GlassSphere;

typedef struct {
    GlassSphere glass;
    // We can add more terrarium-related components here later
    // Like ground, atmosphere effects, etc.
} TerrariumSystem;

TerrariumSystem InitializeTerrariumSystem(Shader glassShader);
void DrawTerrariumSystem(TerrariumSystem* terrarium, Camera3D camera);
void UnloadTerrariumSystem(TerrariumSystem* terrarium);

#endif // TERRARIUM_H