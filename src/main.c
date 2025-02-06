#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <math.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Alien Egg");
    SetTargetFPS(60);

    // Load shaders
    Shader shader = LoadShader("shaders/vertex.glsl", "shaders/fragment.glsl"); 

    // Model setup
    float modelScale = 1.0f;
    Model eggModel = LoadModel("assets/egg.glb");
    eggModel.transform = MatrixScale(modelScale, modelScale, modelScale);
    
    // Assign shader to all materials
    for(int i = 0; i < eggModel.materialCount; i++) {
        eggModel.materials[i].shader = shader;
    }

    // Camera setup
    Camera3D camera = {
        .position = (Vector3){ 0.0f, 0.05f, 0.15f },
        .target = (Vector3){ 0.0f, 0.0f, 0.0f },
        .up = (Vector3){ 0.0f, 1.0f, 0.0f },
        .fovy = 40.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    // Orbital camera parameters
    float cameraDistance = 0.15f;
    float angleHorizontal = 0.0f;
    float angleVertical = 0.3f;
    float rotationSpeed = 2.0f;

    // Get shader locations
    int resolutionLoc = GetShaderLocation(shader, "iResolution");
    int timeLoc = GetShaderLocation(shader, "iTime");
    int cameraPosLoc = GetShaderLocation(shader, "cameraPos");
    int cameraFrontLoc = GetShaderLocation(shader, "cameraFront");
    int cameraUpLoc = GetShaderLocation(shader, "cameraUp");
    int colorLoc = GetShaderLocation(shader, "color");
    int shaderTypeLoc = GetShaderLocation(shader, "shaderType");
    
    // New shader locations
    int modelLoc = GetShaderLocation(shader, "model");
    int mvpLoc = GetShaderLocation(shader, "mvp");
    int normalMatrixLoc = GetShaderLocation(shader, "normalMatrix");

    DisableCursor();

    while (!WindowShouldClose()) {
        // Camera movement
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouseDelta = GetMouseDelta();
            angleHorizontal -= mouseDelta.x * rotationSpeed * GetFrameTime();
            angleVertical -= mouseDelta.y * rotationSpeed * GetFrameTime();
            angleVertical = Clamp(angleVertical, -1.5f, 1.5f);
        }

        // Update camera position
        float x = cameraDistance * cosf(angleVertical) * sinf(angleHorizontal);
        float y = cameraDistance * sinf(angleVertical);
        float z = cameraDistance * cosf(angleVertical) * cosf(angleHorizontal);

        camera.position = (Vector3){ x, y + 0.05f, z };
        camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };

        BeginDrawing();
            ClearBackground(DARKGRAY);
            BeginMode3D(camera);

            // Update basic uniforms
            float screenWidth = (float)GetScreenWidth();
            float screenHeight = (float)GetScreenHeight();
            SetShaderValue(shader, resolutionLoc, (float[2]){screenWidth, screenHeight}, SHADER_UNIFORM_VEC2);
            SetShaderValue(shader, timeLoc, (float[]){GetTime()}, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, cameraPosLoc, (float*)&camera.position, SHADER_UNIFORM_VEC3);
            
            Vector3 cameraFront = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
            SetShaderValue(shader, cameraFrontLoc, (float*)&cameraFront, SHADER_UNIFORM_VEC3);
            SetShaderValue(shader, cameraUpLoc, (float*)&camera.up, SHADER_UNIFORM_VEC3);

            // Draw ground
            DrawPlane((Vector3){0, -2, 0}, (Vector2){20, 20}, BROWN);

            // Setup matrices for each egg
            Vector3 noColor = {0, 0, 0};
            SetShaderValue(shader, colorLoc, (float*)&noColor, SHADER_UNIFORM_VEC3);

            // First egg
            {
                SetShaderValue(shader, shaderTypeLoc, (int[]){0}, SHADER_UNIFORM_INT);
                Vector3 position = {0.02f, -0.01f, 0.0f};
                
                Matrix model = MatrixMultiply(
                    MatrixTranslate(position.x, position.y, position.z),
                    MatrixScale(modelScale, modelScale, modelScale)
                );
                
                Matrix view = GetCameraMatrix(camera);
                Matrix projection = MatrixPerspective(
                    camera.fovy * DEG2RAD,
                    (float)screenWidth/(float)screenHeight,
                    0.01f, 1000.0f
                );
                
                Matrix mvp = MatrixMultiply(MatrixMultiply(model, view), projection);
                Matrix normalMatrix = MatrixTranspose(MatrixInvert(model));
                
                SetShaderValueMatrix(shader, modelLoc, model);
                SetShaderValueMatrix(shader, mvpLoc, mvp);
                SetShaderValueMatrix(shader, normalMatrixLoc, normalMatrix);
                
                DrawModel(eggModel, position, 1.0f, WHITE);
            
                
            }

            // Second egg
            {
                SetShaderValue(shader, shaderTypeLoc, (int[]){1}, SHADER_UNIFORM_INT);
                Vector3 position = {-0.02f, -0.01f, 0.0f};
                
                Matrix model = MatrixMultiply(
                    MatrixTranslate(position.x, position.y, position.z),
                    MatrixScale(modelScale, modelScale, modelScale)
                );
                
                Matrix view = GetCameraMatrix(camera);
                Matrix projection = MatrixPerspective(
                    camera.fovy * DEG2RAD,
                    (float)screenWidth/(float)screenHeight,
                    0.01f, 1000.0f
                );
                
                Matrix mvp = MatrixMultiply(MatrixMultiply(model, view), projection);
                Matrix normalMatrix = MatrixTranspose(MatrixInvert(model));
                
                SetShaderValueMatrix(shader, modelLoc, model);
                SetShaderValueMatrix(shader, mvpLoc, mvp);
                SetShaderValueMatrix(shader, normalMatrixLoc, normalMatrix);
                
                DrawModel(eggModel, position, 1.0f, WHITE);
                    
            }


            EndMode3D();
            
            DrawText("Hold left mouse button and drag to rotate camera", 10, 10, 20, WHITE);
            
        EndDrawing();
    }

    UnloadModel(eggModel);
    UnloadShader(shader);
    CloseWindow();

    return 0;
}