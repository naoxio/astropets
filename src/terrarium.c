#include "terrarium.h"
#include "rlgl.h"
#include "constants.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static Mesh GenerateGroundMesh(float sphereRadius) {
    float groundRadius = sqrtf(sphereRadius * sphereRadius - 1.0f); // Width at y=0
    const int rings = 32;
    const int slices = 32;

    // Vertex and index counts
    int curvedVertexCount = (rings / 2 + 1) * (slices + 1); // Include seam duplication
    int curvedTriangleCount = (rings / 2) * slices * 2;
    int topCapVertexCount = slices + 1; // Center vertex + outer vertices
    int topCapTriangleCount = slices;

    int totalVertexCount = curvedVertexCount + topCapVertexCount;
    int totalTriangleCount = curvedTriangleCount + topCapTriangleCount;

    Mesh mesh = {0};
    mesh.vertices = (float*)MemAlloc(totalVertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)MemAlloc(totalVertexCount * 2 * sizeof(float));
    mesh.normals = (float*)MemAlloc(totalVertexCount * 3 * sizeof(float));
    mesh.indices = (unsigned short*)MemAlloc(totalTriangleCount * 3 * sizeof(unsigned short));

    int vCounter = 0;
    int iCounter = 0;

    // Generate curved bottom section
    for (int i = 0; i <= rings / 2; i++) {
        float t = (float)i / (rings / 2);
        float y = -t;
        float levelRadius = sqrtf(sphereRadius * sphereRadius - (y - 1.0f) * (y - 1.0f));
        for (int j = 0; j <= slices; j++) { // <= ensures the seam is closed
            float theta = 2.0f * PI * ((float)j / slices);
            float x = levelRadius * cosf(theta);
            float z = levelRadius * sinf(theta);

            // Vertices
            mesh.vertices[3 * vCounter] = x;
            mesh.vertices[3 * vCounter + 1] = y;
            mesh.vertices[3 * vCounter + 2] = z;

            // Normals
            float nx = x;
            float ny = y - 1.0f;
            float nz = z;
            float nlen = sqrtf(nx * nx + ny * ny + nz * nz);
            mesh.normals[3 * vCounter] = nx / nlen;
            mesh.normals[3 * vCounter + 1] = ny / nlen;
            mesh.normals[3 * vCounter + 2] = nz / nlen;

            // Texture coordinates
            mesh.texcoords[2 * vCounter] = (float)j / slices;
            mesh.texcoords[2 * vCounter + 1] = t;

            vCounter++;
        }
    }


    // Generate indices for curved section
    int indexOffset = 0;
    for (int i = 0; i < rings / 2; i++) {
        for (int j = 0; j < slices; j++) {
            int base = i * (slices + 1) + j;
            // First triangle
            mesh.indices[indexOffset++] = base;
            mesh.indices[indexOffset++] = base + slices + 1;
            mesh.indices[indexOffset++] = base + 1;
            // Second triangle
            mesh.indices[indexOffset++] = base + 1;
            mesh.indices[indexOffset++] = base + slices + 1;
            mesh.indices[indexOffset++] = base + slices + 2;
        }
    }
    iCounter = indexOffset / 3;  // Update iCounter for top cap indices

    // Generate top cap
    int topCapCenterIndex = vCounter;
    mesh.vertices[3 * vCounter] = 0.0f; // Center vertex
    mesh.vertices[3 * vCounter + 1] = 0.0f;
    mesh.vertices[3 * vCounter + 2] = 0.0f;
    mesh.normals[3 * vCounter] = 0.0f; // Upward normal
    mesh.normals[3 * vCounter + 1] = 1.0f;
    mesh.normals[3 * vCounter + 2] = 0.0f;
    mesh.texcoords[2 * vCounter] = 0.5f; // Texture center
    mesh.texcoords[2 * vCounter + 1] = 0.5f;
    vCounter++;

    for (int j = 0; j < slices; j++) {
        float theta = 2.0f * PI * ((float)j / slices);
        float x = groundRadius * cosf(theta);
        float z = groundRadius * sinf(theta);

        // Vertices
        mesh.vertices[3 * vCounter] = x;
        mesh.vertices[3 * vCounter + 1] = 0.0f;
        mesh.vertices[3 * vCounter + 2] = z;

        // Normals
        mesh.normals[3 * vCounter] = 0.0f; // Upward normal
        mesh.normals[3 * vCounter + 1] = 1.0f;
        mesh.normals[3 * vCounter + 2] = 0.0f;

        // Texture coordinates
        mesh.texcoords[2 * vCounter] = 0.5f + 0.5f * cosf(theta);
        mesh.texcoords[2 * vCounter + 1] = 0.5f + 0.5f * sinf(theta);

        vCounter++;
    }

    // Generate indices for top cap
    for (int j = 0; j < slices; j++) {
        mesh.indices[3 * iCounter] = topCapCenterIndex;
        mesh.indices[3 * iCounter + 1] = topCapCenterIndex + j + 1;
        if (j < slices - 1) {
            mesh.indices[3 * iCounter + 2] = topCapCenterIndex + j + 2;
        } else {
            mesh.indices[3 * iCounter + 2] = topCapCenterIndex + 1; // Connect back to first vertex
        }
        iCounter++;
    }
    // Finalize mesh
    mesh.vertexCount = totalVertexCount;
    mesh.triangleCount = totalTriangleCount;
    UploadMesh(&mesh, false);
    return mesh;
}

Shader LoadGlassShader(const char* vertexFile, const char* fragmentFile) {
    Shader shader = LoadShader(vertexFile, fragmentFile);

    // Get uniform locations
    int albedoColorLoc = GetShaderLocation(shader, "albedoColor");
    int edgeColorLoc = GetShaderLocation(shader, "edgeColor");
    int roughnessValueLoc = GetShaderLocation(shader, "roughnessValue");
    int normalStrengthLoc = GetShaderLocation(shader, "normalStrength");
    int lightDirLoc = GetShaderLocation(shader, "lightDir");
    int normalMatrixLoc = GetShaderLocation(shader, "matNormal");
    int internalLightPosLoc = GetShaderLocation(shader, "internalLightPos");
    int internalLightColorLoc = GetShaderLocation(shader, "internalLightColor");
    int internalLightIntensityLoc = GetShaderLocation(shader, "internalLightIntensity");

    // Further modified values for more transparency and brightness
    float albedoColor[4] = {1.0f, 1.0f, 1.0f, 0.1f};  // Much more transparent base color
    float edgeColor[4] = {1.0f, 1.0f, 1.0f, 0.2f};       // More transparent edges
    float roughnessValue = 0.02f;                         // Even smoother surface
    float normalStrength = 1.0f;                         
    float lightDir[3] = {-0.5f, 1.0f, -0.5f};           
    float internalLightPos[3] = {0.0f, 1.8f, 0.0f};     
    float internalLightColor[3] = {1.0f, 1.0f, 1.0f};    // Pure white light for maximum brightness
    float internalLightIntensity = 12.0f;                 // Significantly increased light intensity

    Matrix normalMatrix = MatrixIdentity();

    SetShaderValue(shader, albedoColorLoc, albedoColor, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, edgeColorLoc, edgeColor, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, roughnessValueLoc, &roughnessValue, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, normalStrengthLoc, &normalStrength, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, lightDirLoc, lightDir, SHADER_UNIFORM_VEC3);
    SetShaderValueMatrix(shader, normalMatrixLoc, normalMatrix);
    SetShaderValue(shader, internalLightPosLoc, internalLightPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, internalLightColorLoc, internalLightColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, internalLightIntensityLoc, &internalLightIntensity, SHADER_UNIFORM_FLOAT);

    return shader;
}

static GlassSphere InitializeGlassSphere(Shader glassShader) {
    GlassSphere glass = {
        .sphere = LoadModelFromMesh(GenMeshSphere(2.0f, 32, 32)),
        .radius = 2.0f,
        .position = (Vector3){0.0f, 1.0f, 0.0f},
        .shader = glassShader
    };
    glass.sphere.materials[0].shader = glassShader;
    return glass;
}

// Initialize the ground
static Ground InitializeGround(Shader groundShader, float sphereRadius) {
    Ground ground = {0};
    Mesh groundMesh = GenerateGroundMesh(sphereRadius);
    ground.surface = LoadModelFromMesh(groundMesh);
    ground.height = 0.0f;  // Place at origin
    ground.shader = groundShader;
    ground.surface.materials[0].shader = groundShader;

    return ground;
}

TerrariumSystem InitializeTerrariumSystem(Shader glassShader, Shader groundShader) {
    TerrariumSystem terrarium = {0};
    terrarium.glass = InitializeGlassSphere(glassShader);
    terrarium.ground = InitializeGround(groundShader, 2.0f);

    // Initialize internal light
    terrarium.internalLight = CreateLight(
        (Vector3){0.0f, 2.5f, 0.0f},  // Position above the sphere
        (Vector3){1.0f, 1.0f, 1.0f},  // White light
        8.0f                          // Intensity
    );

    return terrarium;
}

void UpdateTerrariumLight(TerrariumSystem* terrarium, Vector3 color, float intensity) {
    UpdateLight(&terrarium->internalLight, terrarium->internalLight.position, color, intensity);
}
void DrawTerrariumSystem(TerrariumSystem* terrarium, Camera3D camera) {
    // Disable backface culling for the ground
    rlDisableBackfaceCulling();

    // Draw the ground at the glass sphere's position but offset slightly lower
    Vector3 groundPosition = terrarium->glass.position;
    groundPosition.y -= 0.9f; // Offset by -1 to place the top edge at y=0
    DrawModel(terrarium->ground.surface, groundPosition, 1.0f, WHITE);
    DrawModelWires(terrarium->ground.surface, groundPosition, 1.0f, RED);

    // Re-enable backface culling for other objects
    rlEnableBackfaceCulling();

    // Enable blending for the glass material
    BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);

    // Update shader uniforms for the glass sphere
    float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    int viewPosLoc = GetShaderLocation(terrarium->glass.shader, "viewPos");
    SetShaderValue(terrarium->glass.shader, viewPosLoc, cameraPos, SHADER_UNIFORM_VEC3);

    // Calculate and update the normal matrix for the glass sphere
    Matrix modelMatrix = MatrixTranslate(
        terrarium->glass.position.x,
        terrarium->glass.position.y,
        terrarium->glass.position.z
    );
    Matrix normalMatrix = MatrixTranspose(MatrixInvert(modelMatrix));
    int normalMatrixLoc = GetShaderLocation(terrarium->glass.shader, "matNormal");
    SetShaderValueMatrix(terrarium->glass.shader, normalMatrixLoc, normalMatrix);

    // Update internal light position relative to the sphere
    Vector3 internalLightPos = {
        terrarium->glass.position.x,
        terrarium->glass.position.y + 1.5f, // Moved higher inside the sphere
        terrarium->glass.position.z
    };
    float lightPos[3] = {internalLightPos.x, internalLightPos.y, internalLightPos.z};
    int internalLightPosLoc = GetShaderLocation(terrarium->glass.shader, "internalLightPos");
    SetShaderValue(terrarium->glass.shader, internalLightPosLoc, lightPos, SHADER_UNIFORM_VEC3);

    // Update internal light color and intensity
    float lightColor[3] = {
        terrarium->internalLight.color.x,
        terrarium->internalLight.color.y,
        terrarium->internalLight.color.z
    };
    int internalLightColorLoc = GetShaderLocation(terrarium->glass.shader, "internalLightColor");
    int internalLightIntensityLoc = GetShaderLocation(terrarium->glass.shader, "internalLightIntensity");
    SetShaderValue(terrarium->glass.shader, internalLightColorLoc, lightColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(terrarium->glass.shader, internalLightIntensityLoc, &terrarium->internalLight.intensity, SHADER_UNIFORM_FLOAT);

    // Draw a small sphere to represent the internal light source
    DrawSphere(internalLightPos, 0.1f, YELLOW);

    // Draw the glass sphere
    DrawModel(terrarium->glass.sphere, terrarium->glass.position, 1.0f, WHITE);

    // Disable blending after drawing the glass sphere
    EndBlendMode();
}
// Unload resources
void UnloadTerrariumSystem(TerrariumSystem* terrarium) {
    UnloadModel(terrarium->glass.sphere);
    UnloadModel(terrarium->ground.surface);
}