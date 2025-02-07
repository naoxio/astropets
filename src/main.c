#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <rlgl.h>
#include "hay.h"
#include "egg.h" 
#include "constants.h"
#include "terrarium.h"

int main(void) {
   const int screenWidth = 800;
   const int screenHeight = 600;

   // Initialize window
   InitWindow(screenWidth, screenHeight, "Space Terrarium");
   SetTargetFPS(60);

   // Load shaders
   Shader eggShader = LoadShader("shaders/egg_vertex.glsl", "shaders/egg_fragment.glsl");
   Shader glassShader = LoadShader("shaders/glass_vertex.glsl", "shaders/glass_fragment.glsl");
   Shader groundShader = LoadShader("shaders/ground_vertex.glsl", "shaders/ground_fragment.glsl");
   Shader spaceShader = LoadShader("shaders/space_vertex.glsl", "shaders/space_background.fs");

   // Create skybox
   Mesh skyMesh = GenMeshSphere(1000.0f, 64, 64);
   Model skybox = LoadModelFromMesh(skyMesh);
   skybox.materials[0].shader = spaceShader;

   // Create a center point that everything will reference
   Vector3 centerPoint = (Vector3){ 0.0f, 0.0f, 0.0f };

   // Initialize systems
   HayPiece* hayPieces = InitializeNest();
   EggSystem eggSystem = InitializeEggSystem(eggShader);
   TerrariumSystem terrarium = InitializeTerrariumSystem(glassShader, groundShader);

   // Camera setup centered on centerPoint
   Camera3D camera = {
       .position = (Vector3){ centerPoint.x, centerPoint.y + 2.0f, centerPoint.z + 4.0f },
       .target = centerPoint,
       .up = (Vector3){ 0.0f, 1.0f, 0.0f },
       .fovy = 40.0f,
       .projection = CAMERA_PERSPECTIVE,
   };

   // Orbital camera parameters
   float cameraDistance = 4.0f;
   const float minDistance = 3.0f;
   const float maxDistance = 10.0f;
   const float zoomSpeed = 0.5f;
   float angleHorizontal = 0.0f;
   float angleVertical = 0.3f;
   float rotationSpeed = 2.0f;

   DisableCursor();

   while (!WindowShouldClose()) {
       float deltaTime = GetFrameTime();

       // Handle zoom with mouse wheel
       float wheel = GetMouseWheelMove();
       if (wheel != 0) {
           cameraDistance -= wheel * zoomSpeed;
           cameraDistance = Clamp(cameraDistance, minDistance, maxDistance);
       }

       // Spawn new egg on spacebar
       if (IsKeyPressed(KEY_SPACE)) {
           SpawnEgg(&eggSystem);
       }

       if (IsKeyPressed(KEY_L)) {  // Press L to increase light intensity
           terrarium.internalLight.intensity += 2.0f;
       }
       if (IsKeyPressed(KEY_K)) {  // Press K to decrease light intensity
           terrarium.internalLight.intensity -= 2.0f;
           terrarium.internalLight.intensity = fmax(0.0f, terrarium.internalLight.intensity);
       }

       // Update physics
       UpdateEggPhysics(&eggSystem, hayPieces, deltaTime);

       // Camera movement
       if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
           Vector2 mouseDelta = GetMouseDelta();
           angleHorizontal -= mouseDelta.x * rotationSpeed * deltaTime;
           angleVertical -= mouseDelta.y * rotationSpeed * deltaTime;
           angleVertical = Clamp(angleVertical, -1.5f, 1.5f);
       }

       // Update camera position relative to center
       float x = centerPoint.x + cameraDistance * cosf(angleVertical) * sinf(angleHorizontal);
       float y = centerPoint.y + cameraDistance * sinf(angleVertical);
       float z = centerPoint.z + cameraDistance * cosf(angleVertical) * cosf(angleHorizontal);
       camera.position = (Vector3){ x, y + 0.05f, z };
       camera.target = centerPoint;

       BeginDrawing();
           ClearBackground(BLACK);

           BeginMode3D(camera);
               // Draw skybox first
               rlDisableBackfaceCulling();
               rlDisableDepthMask();
               rlDisableDepthTest(); 

               // Update shader uniforms
               float timeValue = GetTime();
               SetShaderValue(spaceShader, GetShaderLocation(spaceShader, "time"), &timeValue, SHADER_UNIFORM_FLOAT);
               
               // Draw skybox centered
               DrawModel(skybox, centerPoint, 1.0f, WHITE);
               
               rlEnableDepthTest();
               rlEnableBackfaceCulling();
               rlEnableDepthMask();

               // Draw contents (hay and eggs) relative to center
               for (int i = 0; i < NUM_HAY_PIECES + TOP_LAYER_PIECES; i++) {
                   if (Vector3Distance(hayPieces[i].startPos, centerPoint) < terrarium.glass.radius) {
                       DrawHayPiece(hayPieces[i]);
                   }
               }

               DrawEgg(&eggSystem, camera, eggShader);
               DrawTerrariumSystem(&terrarium, camera);
           EndMode3D();

           DrawText("Hold left mouse button and drag to rotate camera", 10, 10, 20, WHITE);
           DrawText("Use mouse wheel to zoom in/out", 10, 30, 20, WHITE);
           DrawText("Press SPACE to spawn egg", 10, 50, 20, WHITE);
           DrawText("Press L/K to increase/decrease light", 10, 70, 20, WHITE);
       EndDrawing();
   }

   // Cleanup
   UnloadModel(skybox);
   free(hayPieces);
   UnloadEggSystem(&eggSystem);
   UnloadShader(eggShader);
   UnloadShader(glassShader);
   UnloadShader(groundShader);
   UnloadShader(spaceShader);
   UnloadTerrariumSystem(&terrarium);
   CloseWindow();

   return 0;
}