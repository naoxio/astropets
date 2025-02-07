#version 330
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
out vec4 finalColor;

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    // Base soil color (darker brown)
    vec3 baseColor = vec3(0.25, 0.16, 0.1);
    // Variation color (lighter brown)
    vec3 varColor = vec3(0.35, 0.22, 0.15);

    // Simple noise for variation
    float noise = rand(fragTexCoord) * 0.3;

    vec3 groundColor = mix(baseColor, varColor, noise);

    // Lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(fragNormal, lightDir), 0.2);
    float ambient = 0.3;

    finalColor = vec4(groundColor * (diff + ambient), 1.0);
}