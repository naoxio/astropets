#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader_loader.h"
#include "model_loader.h"
#include <cglm/cglm.h>

// Camera variables
vec3 cameraPos = { 0.0f, 0.0f, 0.1f };
vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
vec3 cameraUp = { 0.0f, 1.0f, 0.0f };
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;

// Movement speed
const float cameraSpeed = 0.001f;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float currentSpeed = cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        currentSpeed *= 2.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_scale(cameraFront, currentSpeed, temp);
        glm_vec3_add(cameraPos, temp, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_scale(cameraFront, currentSpeed, temp);
        glm_vec3_sub(cameraPos, temp, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 cross;
        glm_vec3_cross(cameraFront, cameraUp, cross);
        glm_normalize(cross);
        glm_vec3_scale(cross, currentSpeed, cross);
        glm_vec3_sub(cameraPos, cross, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 cross;
        glm_vec3_cross(cameraFront, cameraUp, cross);
        glm_normalize(cross);
        glm_vec3_scale(cross, currentSpeed, cross);
        glm_vec3_add(cameraPos, cross, cameraPos);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    vec3 front;
    front[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
    front[1] = sin(glm_rad(pitch));
    front[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
    glm_normalize(front);
    glm_vec3_copy(front, cameraFront);
}

int main() {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Alien Egg", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    // Initialize viewport
    glViewport(0, 0, 800, 600);

    // Mouse and input setup
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load and compile shaders
    char* vertexSource = load_shader("shaders/vertex.glsl");
    char* fragmentSource = load_shader("shaders/fragment.glsl");
    if (!vertexSource || !fragmentSource) {
        printf("Failed to load shaders\n");
        return -1;
    }

    GLuint shaderProgram = create_shader_program(vertexSource, fragmentSource);
    if (!shaderProgram) {
        printf("Failed to create shader program\n");
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Load model
    float modelScale = 1.0f;
    Model eggModel = load_model("assets/egg.glb", modelScale);
    if (eggModel.vao == 0) {
        printf("Failed to load model\n");
        return -1;
    }

    // Create and setup ground plane
    float groundVertices[] = {
        -10.0f, -2.0f, -10.0f,
         10.0f, -2.0f, -10.0f,
         10.0f, -2.0f,  10.0f,
        -10.0f, -2.0f,  10.0f
    };
    unsigned int groundIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint groundVAO, groundVBO, groundEBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);

    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Set common uniforms
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glUniform2f(glGetUniformLocation(shaderProgram, "iResolution"), (float)width, (float)height);
        glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), (float)glfwGetTime());
        glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, cameraPos);
        glUniform3fv(glGetUniformLocation(shaderProgram, "cameraFront"), 1, cameraFront);
        glUniform3fv(glGetUniformLocation(shaderProgram, "cameraUp"), 1, cameraUp);

        // Create view and projection matrices
        mat4 projection;
        glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);

        mat4 view;
        vec3 center;
        glm_vec3_add(cameraPos, cameraFront, center);
        glm_lookat(cameraPos, center, cameraUp, view);

        // Set matrix uniforms
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (float*)view);

        // Draw ground
        mat4 groundModel;
        glm_mat4_identity(groundModel);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)groundModel);
        vec3 groundColor = {0.45f, 0.29f, 0.14f};
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, groundColor);
        glBindVertexArray(groundVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Draw first egg (Shader Type 0)
        glUniform1i(glGetUniformLocation(shaderProgram, "shaderType"), 0);
        mat4 modelMatrix1;
        glm_mat4_identity(modelMatrix1);
        glm_translate(modelMatrix1, (vec3){0.02f, -0.01f, 0.f}); // Position first egg
        glm_scale(modelMatrix1, (vec3){modelScale, modelScale, modelScale});
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)modelMatrix1);
        vec3 noColor = {0.0f, 0.0f, 0.0f};
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, noColor);
        glBindVertexArray(eggModel.vao);
        glDrawElements(GL_TRIANGLES, eggModel.indexCount, GL_UNSIGNED_INT, 0);

        // Draw second egg (Shader Type 1)
        glUniform1i(glGetUniformLocation(shaderProgram, "shaderType"), 1);
        mat4 modelMatrix2;
        glm_mat4_identity(modelMatrix2);
        glm_translate(modelMatrix2, (vec3){-0.02f, -0.01f, 0.0f}); // Position second egg
        glm_scale(modelMatrix2, (vec3){modelScale, modelScale, modelScale});
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)modelMatrix2);
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, noColor);
        glBindVertexArray(eggModel.vao);
        glDrawElements(GL_TRIANGLES, eggModel.indexCount, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &groundEBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}