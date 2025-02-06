#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

// Input uniform matrices
uniform mat4 mvp;
uniform mat4 model;
uniform mat4 normalMatrix;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
    // Calculate fragment position in world space
    fragPosition = vec3(model * vec4(vertexPosition, 1.0));
    
    // Calculate normal in world space
    fragNormal = normalize(mat3(normalMatrix) * vertexNormal);
    
    // Pass the texture coordinates
    fragTexCoord = vertexTexCoord;
    
    // Calculate final vertex position
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}