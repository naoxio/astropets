#version 330

// Input attributes
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

// Uniforms
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;    // Normal matrix (add this to your C code)

// Outputs to fragment shader
out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main() {
    // Transform position to world space
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    
    // Transform normal using normal matrix for correct lighting
    fragNormal = normalize(mat3(matNormal) * vertexNormal);
    
    // Pass texture coordinates
    fragTexCoord = vertexTexCoord;
    
    // Final position
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}