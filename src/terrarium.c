#include "terrarium.h"
#include "rlgl.h"

static GlassSphere InitializeGlassSphere(Shader glassShader) {
    GlassSphere glass = {
        .sphere = LoadModelFromMesh(GenMeshSphere(2.0f, 32, 32)),
        .radius = 2.0f,
        .position = (Vector3){ 0.0f, 1.0f, 0.0f },
        .shader = glassShader
    };
    glass.sphere.materials[0].shader = glassShader;
    return glass;
}

TerrariumSystem InitializeTerrariumSystem(Shader glassShader) {
    TerrariumSystem terrarium = {
        .glass = InitializeGlassSphere(glassShader)
    };
    return terrarium;
}

void DrawTerrariumSystem(TerrariumSystem* terrarium, Camera3D camera) {
    BeginBlendMode(BLEND_ALPHA);
    
    float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
    SetShaderValue(terrarium->glass.shader, 
                  GetShaderLocation(terrarium->glass.shader, "viewPos"), 
                  cameraPos, SHADER_UNIFORM_VEC3);
    
    DrawModel(terrarium->glass.sphere, 
             terrarium->glass.position, 
             1.0f, Fade(WHITE, 0.5f));
    
    EndBlendMode();
}

void UnloadTerrariumSystem(TerrariumSystem* terrarium) {
    UnloadModel(terrarium->glass.sphere);
    // Add any future cleanup here
}
