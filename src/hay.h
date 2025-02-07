#ifndef HAY_H
#define HAY_H

#include <raylib.h>
#include <raymath.h>
#include "constants.h"

#define NUM_HAY_PIECES 1000
#define NEST_RADIUS 0.4f
#define NEST_HEIGHT 0.2f
#define TOP_LAYER_PIECES 300
#define HAY_STIFFNESS 5.0f     
#define HAY_DAMPING 0.5f        
#define MAX_COMPRESSION 0.15f   

typedef struct {
    Vector3 startPos;
    Vector3 endPos;
    Vector3 controlPoint;
    Vector3 originalHeight;  
    float compression;         
    float radius;
    Color color;
} HayPiece;

typedef struct {
    Vector3 position;
    float radius;
    bool active;
} CollisionSphere;

HayPiece* InitializeNest(void);
void DrawHayPiece(HayPiece hay);
float GetRandomFloat(float min, float max);
void UpdateHayPhysics(HayPiece* hayPieces, CollisionSphere egg, float deltaTime);
float CalculateHayHeight(Vector3 position, HayPiece* hayPieces);

#endif