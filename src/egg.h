#ifndef EGG_H
#define EGG_H

#include <raylib.h>
#include "hay.h"
#include "constants.h"

typedef struct {
    Vector3 position;
    Vector3 velocity;
    bool isGrounded;
    int colorType;
    bool active;
} PhysicsObject;

typedef struct {
    Model model;
    PhysicsObject egg;
    int numColors;
} EggSystem;

EggSystem InitializeEggSystem(Shader shader);
void SpawnEgg(EggSystem* eggSystem, int colorType);
void UpdateEggPhysics(EggSystem* eggSystem, HayPiece* hayPieces, float deltaTime);
void DrawEgg(EggSystem* eggSystem, Camera3D camera, Shader shader);
void UnloadEggSystem(EggSystem* eggSystem);

#endif // EGG_H