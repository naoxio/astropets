#include "hay.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

float GetRandomFloat(float min, float max) {
    float random = (float)GetRandomValue((int)(min * 1000), (int)(max * 1000));
    return random / 1000.0f;
}

static float QuadraticBezier(float start, float control, float end, float t) {
    float one_minus_t = 1.0f - t;
    return one_minus_t * one_minus_t * start + 
           2.0f * one_minus_t * t * control + 
           t * t * end;
}
void UpdateHayPhysics(HayPiece* hayPieces, CollisionSphere egg, float deltaTime) {
    const float EGG_WEIGHT = 1.0f;       // Weight of the egg affecting the hay
    const float DECOMPRESS_RATE = 0.5f; // Rate at which hay decompresses (adjust as needed)

    if (egg.radius <= 0) {
        // No egg - allow natural decompression without resetting to originalHeight
        for (int i = 0; i < NUM_HAY_PIECES + TOP_LAYER_PIECES; i++) {
            // Gradual decompression
            if (hayPieces[i].compression > 0) {
                hayPieces[i].compression -= DECOMPRESS_RATE * deltaTime;
                if (hayPieces[i].compression < 0) {
                    hayPieces[i].compression = 0; // Ensure compression doesn't go negative
                }

                // Update Y positions based on remaining compression
                hayPieces[i].startPos.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;
                hayPieces[i].endPos.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;
                hayPieces[i].controlPoint.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;

            }
        }
        return;
    }

    // Egg is present - update physics for each hay piece
    for (int i = 0; i < NUM_HAY_PIECES + TOP_LAYER_PIECES; i++) {
        float dx = hayPieces[i].startPos.x - egg.position.x;
        float dz = hayPieces[i].startPos.z - egg.position.z;
        float distance = sqrtf(dx * dx + dz * dz);

        float eggBottom = egg.position.y - egg.radius;

        // Check if the egg is above the hay piece and within its radius
        if (eggBottom <= hayPieces[i].originalHeight.y && 
            egg.position.y > hayPieces[i].originalHeight.y && 
            distance < egg.radius) {


            // Calculate weight factor based on distance from the egg's center
            float weight_factor = EGG_WEIGHT * (1.0f - (distance / egg.radius));
            hayPieces[i].compression += weight_factor * deltaTime;

            // Clamp compression to a maximum value
            if (hayPieces[i].compression > MAX_COMPRESSION) {
                hayPieces[i].compression = MAX_COMPRESSION;
            }

            // Update Y positions based on current compression
            hayPieces[i].startPos.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;
            hayPieces[i].endPos.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;
            hayPieces[i].controlPoint.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;

        } else {
            // Gradual decompression when the egg is not affecting this piece
            if (hayPieces[i].compression > 0) {
                hayPieces[i].compression -= DECOMPRESS_RATE * deltaTime;
                if (hayPieces[i].compression < 0) {
                    hayPieces[i].compression = 0; // Ensure compression doesn't go negative
                }

                // Update Y positions based on remaining compression
                hayPieces[i].startPos.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;
                hayPieces[i].endPos.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;
                hayPieces[i].controlPoint.y = hayPieces[i].originalHeight.y - hayPieces[i].compression;

            }
        }
    }
}


float CalculateHayHeight(Vector3 position, HayPiece* hayPieces) {
    float maxHeight = GROUND_Y;
    float weightedSum = 0;
    float totalWeight = 0;

    for (int i = 0; i < NUM_HAY_PIECES + TOP_LAYER_PIECES; i++) {
        float dx = hayPieces[i].startPos.x - position.x;
        float dz = hayPieces[i].startPos.z - position.z;
        float distance = sqrtf(dx * dx + dz * dz);

        if (distance < NEST_RADIUS) {
            float weight = 1.0f / (1.0f + distance);
            weightedSum += (hayPieces[i].startPos.y - hayPieces[i].compression) * weight;
            totalWeight += weight;
        }
    }

    if (totalWeight > 0) {
        maxHeight = weightedSum / totalWeight;
    }

    return maxHeight;
}

HayPiece* InitializeNest(void) {
    HayPiece* hayPieces = (HayPiece*)malloc((NUM_HAY_PIECES + TOP_LAYER_PIECES) * sizeof(HayPiece));

    // Base layer
    for (int i = 0; i < NUM_HAY_PIECES; i++) {
        float angle = GetRandomFloat(0, 2 * PI);
        float radius = GetRandomFloat(NEST_RADIUS * 0.3f, NEST_RADIUS);
        float height = GetRandomFloat(0, NEST_HEIGHT * 0.7f);

        Vector3 basePos = (Vector3){
            sinf(angle) * radius,
            height * (radius / NEST_RADIUS),
            cosf(angle) * radius
        };

        float pieceLength = GetRandomFloat(0.1f, 0.3f);
        float curvature = GetRandomFloat(-0.2f, 0.2f);

        float centerAngle = atan2f(basePos.x, basePos.z);

        hayPieces[i].startPos = basePos;
        hayPieces[i].originalHeight = basePos;
        hayPieces[i].compression = 0;
        hayPieces[i].endPos = (Vector3){
            basePos.x - sinf(centerAngle) * pieceLength * 0.5f,
            basePos.y + GetRandomFloat(-0.05f, 0.05f),
            basePos.z - cosf(centerAngle) * pieceLength * 0.5f
        };
        hayPieces[i].controlPoint = (Vector3){
            (basePos.x + hayPieces[i].endPos.x) / 2 + curvature,
            basePos.y + GetRandomFloat(0.05f, 0.15f),
            (basePos.z + hayPieces[i].endPos.z) / 2 + curvature
        };
        hayPieces[i].radius = GetRandomFloat(0.002f, 0.004f);
        hayPieces[i].color = (Color){
            GetRandomValue(220, 255),
            GetRandomValue(180, 223),
            GetRandomValue(60, 91),
            255
        };

        // Debug output
        printf("HayPiece %d: startPos = (%.3f, %.3f, %.3f), originalHeight = (%.3f, %.3f, %.3f)\n",
               i, hayPieces[i].startPos.x, hayPieces[i].startPos.y, hayPieces[i].startPos.z,
               hayPieces[i].originalHeight.x, hayPieces[i].originalHeight.y, hayPieces[i].originalHeight.z);
    }

    // Top layer
    for (int i = 0; i < TOP_LAYER_PIECES; i++) {
        int idx = NUM_HAY_PIECES + i;
        float angle = GetRandomFloat(0, 2 * PI);
        float radius = GetRandomFloat(0, NEST_RADIUS * 0.6f);
        float height = NEST_HEIGHT * 0.6f + GetRandomFloat(0, NEST_HEIGHT * 0.4f);

        Vector3 basePos = (Vector3){
            sinf(angle) * radius,
            height,
            cosf(angle) * radius
        };

        float centerAngle = atan2f(basePos.x, basePos.z);
        float pieceLength = GetRandomFloat(0.1f, 0.3f);

        hayPieces[idx].startPos = basePos;
        hayPieces[idx].originalHeight = basePos;
        hayPieces[idx].compression = 0;
        hayPieces[idx].endPos = (Vector3){
            basePos.x - sinf(centerAngle) * pieceLength * 0.5f,
            basePos.y + GetRandomFloat(-0.05f, 0.05f),
            basePos.z - cosf(centerAngle) * pieceLength * 0.5f
        };
        hayPieces[idx].controlPoint = (Vector3){
            (basePos.x + hayPieces[idx].endPos.x) / 2,
            basePos.y + GetRandomFloat(0.05f, 0.15f),
            (basePos.z + hayPieces[idx].endPos.z) / 2
        };
        hayPieces[idx].radius = GetRandomFloat(0.002f, 0.004f);
        hayPieces[idx].color = (Color){
            GetRandomValue(220, 255),
            GetRandomValue(180, 223),
            GetRandomValue(60, 91),
            255
        };

        // Debug output
        printf("HayPiece %d: startPos = (%.3f, %.3f, %.3f), originalHeight = (%.3f, %.3f, %.3f)\n",
               idx, hayPieces[idx].startPos.x, hayPieces[idx].startPos.y, hayPieces[idx].startPos.z,
               hayPieces[idx].originalHeight.x, hayPieces[idx].originalHeight.y, hayPieces[idx].originalHeight.z);
    }

    return hayPieces;
}

void DrawHayPiece(HayPiece hay) {
    const int segments = 8;

    for (int i = 0; i < segments - 1; i++) {
        float t1 = (float)i / (segments - 1);
        float t2 = (float)(i + 1) / (segments - 1);

        Vector3 p1 = {
            QuadraticBezier(hay.startPos.x, hay.controlPoint.x, hay.endPos.x, t1),
            QuadraticBezier(hay.startPos.y, hay.controlPoint.y, hay.endPos.y, t1),
            QuadraticBezier(hay.startPos.z, hay.controlPoint.z, hay.endPos.z, t1)
        };

        Vector3 p2 = {
            QuadraticBezier(hay.startPos.x, hay.controlPoint.x, hay.endPos.x, t2),
            QuadraticBezier(hay.startPos.y, hay.controlPoint.y, hay.endPos.y, t2),
            QuadraticBezier(hay.startPos.z, hay.controlPoint.z, hay.endPos.z, t2)
        };

        DrawCylinderEx(p1, p2, hay.radius, hay.radius, 4, hay.color);
    }
}