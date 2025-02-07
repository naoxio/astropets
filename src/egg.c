#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include "egg.h"
#include "hay.h"

EggSystem InitializeEggSystem(Shader shader) {
    EggSystem eggSystem = { 0 };
    eggSystem.model = LoadModel("assets/egg.glb");
    eggSystem.model.transform = MatrixScale(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);

    for (int i = 0; i < eggSystem.model.materialCount; i++) {
        eggSystem.model.materials[i].shader = shader;
    }

    eggSystem.numColors = NUM_COLORS;
    eggSystem.egg.active = false;

    return eggSystem;
}

void SpawnEgg(EggSystem* eggSystem) {
    eggSystem->egg = (PhysicsObject){
        .position = (Vector3){ 0.0f, 2.0f, 0.0f },
        .velocity = (Vector3){ 0.0f, 0.0f, 0.0f },
        .isGrounded = false,
        .colorType = GetRandomValue(0, eggSystem->numColors - 1),
        .active = true
    };
}

void UpdateEggPhysics(EggSystem* eggSystem, HayPiece* hayPieces, float deltaTime) {
    if (!eggSystem->egg.active) return;

    CollisionSphere eggSphere = {
        .position = eggSystem->egg.position,
        .radius = 0.1f
    };

    if (!eggSystem->egg.isGrounded) {
        eggSystem->egg.velocity.y -= GRAVITY * deltaTime;
        eggSystem->egg.position.y += eggSystem->egg.velocity.y * deltaTime;

        float hayHeight = CalculateHayHeight(eggSystem->egg.position, hayPieces);
        if (eggSystem->egg.position.y <= hayHeight) {
            eggSystem->egg.position.y = hayHeight;
            if (fabsf(eggSystem->egg.velocity.y) > 0.1f) {
                eggSystem->egg.velocity.y *= -0.3f;
            } else {
                eggSystem->egg.velocity.y = 0;
                eggSystem->egg.isGrounded = true;
            }
        }
    } else {
        float hayHeight = CalculateHayHeight(eggSystem->egg.position, hayPieces);
        eggSystem->egg.position.y = hayHeight;
        eggSystem->egg.velocity.y = 0;
    }

    UpdateHayPhysics(hayPieces, eggSphere, deltaTime);
}

void DrawEgg(EggSystem* eggSystem, Camera3D camera, Shader shader) {
    if (!eggSystem->egg.active) return;

    Vector3 noColor = {0, 0, 0};
    SetShaderValue(shader, GetShaderLocation(shader, "color"), (float*)&noColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, GetShaderLocation(shader, "shaderType"), (int[]){eggSystem->egg.colorType}, SHADER_UNIFORM_INT);

    Matrix model = MatrixMultiply(
        MatrixTranslate(eggSystem->egg.position.x, eggSystem->egg.position.y, eggSystem->egg.position.z),
        MatrixScale(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE)
    );

    Matrix view = GetCameraMatrix(camera);
    Matrix projection = MatrixPerspective(
        camera.fovy * DEG2RAD,
        (float)GetScreenWidth() / (float)GetScreenHeight(),
        0.01f, 1000.0f
    );

    Matrix mvp = MatrixMultiply(MatrixMultiply(model, view), projection);
    Matrix normalMatrix = MatrixTranspose(MatrixInvert(model));

    SetShaderValueMatrix(shader, GetShaderLocation(shader, "model"), model);
    SetShaderValueMatrix(shader, GetShaderLocation(shader, "mvp"), mvp);
    SetShaderValueMatrix(shader, GetShaderLocation(shader, "normalMatrix"), normalMatrix);

    DrawModel(eggSystem->model, eggSystem->egg.position, 1.0f, WHITE);
}

void UnloadEggSystem(EggSystem* eggSystem) {
    UnloadModel(eggSystem->model);
}