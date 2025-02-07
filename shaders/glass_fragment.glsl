#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform vec4 albedoColor;
uniform vec4 edgeColor;
uniform float roughnessValue; 
uniform float normalStrength;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 internalLightPos;
uniform vec3 internalLightColor;
uniform float internalLightIntensity;
uniform samplerCube environmentMap;

const float IOR = 1.15;
const float F0 = 0.008;
const float TRANSPARENCY = 1.0;
const float EDGE_STRENGTH = 0.8;

float fresnelSchlick(float cosTheta) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(viewPos - fragPosition);
    vec3 L = normalize(lightDir);
    vec3 H = normalize(L + V);

    vec3 refractDir = refract(-V, N, 1.0 / IOR);
    if (dot(refractDir, refractDir) == 0.0) {
        refractDir = reflect(-V, N);
    }
    vec3 reflectDir = reflect(-V, N);

    vec3 toLight = internalLightPos - fragPosition;
    vec3 internalL = normalize(toLight);
    float internalDist = length(toLight);
    float internalAttenuation = exp(-internalDist * 0.1);

    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotIntL = max(dot(N, internalL), 0.0);

    float fresnel = fresnelSchlick(NdotV) * 0.3;
    float edge = pow(1.0 - NdotV, EDGE_STRENGTH) * (1.0 - fresnel) * 0.3;
    float specular = pow(max(dot(N, H), 0.0), 24.0) * fresnel;
    float internalSpecular = pow(max(dot(N, normalize(internalL + refractDir)), 0.0), 24.0) * (1.0 - fresnel);

    // Very subtle base color to preserve transparency
    vec3 baseColor = mix(albedoColor.rgb, vec3(1.0), 0.1);
    vec3 color = baseColor * 0.3;

    // Minimal edge highlighting
    color += edge * edgeColor.rgb * EDGE_STRENGTH * 0.03;

    // Subtle internal lighting
    vec3 internalContribution = internalLightColor * internalLightIntensity * 
                               NdotIntL * internalAttenuation * 1.5;

    // Very soft caustics
    float caustic = pow(max(dot(refractDir, internalL), 0.0), 48.0) * internalAttenuation;
    color += internalLightColor * caustic * internalLightIntensity * 0.1;

    // Minimal environment reflection
    vec3 reflection = reflect(-V, N);
    vec3 envColor = vec3(0.5);
    if (textureSize(environmentMap, 0).x > 0) {
        envColor = texture(environmentMap, reflection).rgb;
    }
    color = mix(color, envColor, fresnel * 0.1);
    
    // Very subtle internal lighting contribution
    color = mix(color, internalContribution, (1.0 - fresnel) * 0.2);

    // Minimal ambient light
    color += vec3(0.02, 0.02, 0.02);

    // Keep original color very subtle
    color *= 0.7;

    // Very high transparency
    float opacity = mix(TRANSPARENCY * 0.05, TRANSPARENCY * 0.15, fresnel);
    opacity += edge * 0.01;
    opacity += specular * 0.01;
    opacity = clamp(opacity, 0.01, 0.1);

    finalColor = vec4(color, opacity);
}