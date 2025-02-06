// glass_fragment.glsl
#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3 viewPos;

out vec4 finalColor;

void main() {
    vec3 I = normalize(fragPosition - viewPos);
    vec3 R = reflect(I, normalize(fragNormal));
    
    // Fresnel effect
    float ratio = 1.00 / 1.52;
    vec3 refracted = refract(I, normalize(fragNormal), ratio);
    
    // Mix reflection and refraction
    float fresnel = pow(1.0 - dot(normalize(fragNormal), -I), 2.0);
    
    vec3 reflectedColor = vec3(0.8, 0.8, 0.9);
    vec3 refractedColor = vec3(0.7, 0.7, 0.8);
    
    vec3 color = mix(refractedColor, reflectedColor, fresnel);
    
    finalColor = vec4(color, 0.3); // Adjust alpha for transparency
}
