#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform float time;
uniform vec2 resolution;

const vec4 BG_COLOR = vec4(0.02, 0.02, 0.05, 1.0);
const float STAR_SIZE = 100.0;
const float STAR_PROB = 0.9;

float rand(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    // Use the fragment position directly for cube mapping
    vec3 normalizedPos = normalize(fragPosition);
    
    // Convert the direction vector to UV coordinates using spherical mapping
    vec2 uv;
    
    // Calculate UV based on which face of the cube we're on
    float absX = abs(normalizedPos.x);
    float absY = abs(normalizedPos.y);
    float absZ = abs(normalizedPos.z);
    
    // Find which axis has the largest magnitude
    float maxAxis = max(max(absX, absY), absZ);
    
    if (maxAxis == absX) {
        // X faces
        uv = vec2(normalizedPos.z / absX, normalizedPos.y / absX);
        uv = uv * 0.5 + 0.5;
    } else if (maxAxis == absY) {
        // Y faces
        uv = vec2(normalizedPos.x / absY, normalizedPos.z / absY);
        uv = uv * 0.5 + 0.5;
    } else {
        // Z faces
        uv = vec2(normalizedPos.x / absZ, normalizedPos.y / absZ);
        uv = uv * 0.5 + 0.5;
    }
    
    // Scale UV coordinates for star placement
    vec2 scaledUV = uv * resolution;
    
    // Calculate star positions
    vec2 pos = floor(scaledUV / STAR_SIZE);
    float starValue = rand(pos);
    
    // Initialize color
    float color = 0.0;
    
    // First type of stars (bright points)
    if (starValue > STAR_PROB) {
        vec2 center = STAR_SIZE * pos + vec2(STAR_SIZE, STAR_SIZE) * 0.5;
        float t = 0.9 + 0.2 * sin(time * 8.0 + (starValue - STAR_PROB) / (1.0 - STAR_PROB) * 45.0);
        float dist = distance(scaledUV, center) / (0.5 * STAR_SIZE);
        color = (1.0 - dist) * t;
        
        vec2 delta = scaledUV - center;
        color = color * t / (abs(delta.y) + 2.0) * t / (abs(delta.x) + 2.0);
    }
    
    // Second type of stars (twinkling background stars)
    else if (rand(uv * 20.0) > 0.996) {
        float r = rand(uv);
        color = r * (0.85 * sin(time * (r * 5.0) + 720.0 * r) + 0.95) * 0.3;
    }
    
    // Add some nebula-like background variation
    float nebula = rand(uv * 4.0) * 0.03;
    
    // Add some color variation based on position
    vec3 colorVar = normalize(abs(normalizedPos)) * 0.1;
    
    // Combine colors
    vec4 finalStarColor = vec4(vec3(color), 1.0);
    vec4 baseColor = BG_COLOR + vec4(nebula + colorVar.r, nebula * 0.5 + colorVar.g, nebula * 2.0 + colorVar.b, 0.0);
    
    finalColor = finalStarColor + baseColor;
    finalColor.a = 1.0;
}