#version 330

// Inputs from vertex shader
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

// Output
out vec4 finalColor;

// Uniforms
uniform vec2 iResolution;
uniform float iTime;
uniform vec3 cameraPos;
uniform vec3 cameraFront;
uniform vec3 cameraUp;
uniform vec3 color;
uniform int shaderType;

// Enhanced noise functions
float hash(float n) { 
    return fract(sin(n) * 753.5453123); 
}

float noise(vec3 p) {
    vec3 fp = floor(p);
    vec3 fr = fract(p);
    fr = fr * fr * (3.0 - 2.0 * fr);
    float n = fp.x + fp.y * 157.0 + 113.0 * fp.z;
    return mix(
        mix(
            mix(hash(n + 0.0), hash(n + 1.0), fr.x),
            mix(hash(n + 157.0), hash(n + 158.0), fr.x),
            fr.y
        ),
        mix(
            mix(hash(n + 113.0), hash(n + 114.0), fr.x),
            mix(hash(n + 270.0), hash(n + 271.0), fr.x),
            fr.y
        ),
        fr.z
    );
}

float fbm(vec3 p) {
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    for (int i = 0; i < 4; i++) {
        value += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main() {
    if (color != vec3(0.0)) {
        finalColor = vec4(color, 1.0);
        return;
    }

    vec3 viewDir = normalize(cameraPos - fragPosition);
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, -1.0));

    float diff = max(dot(normal, lightDir), 0.0);
    float rim = pow(1.0 - max(dot(normal, viewDir), 0.0), 3.0);

    // Add wobble effect
    vec3 p = fragPosition * 20.0;
    vec3 wobble = vec3(
        sin(iTime * 0.5 + p.x * 2.0),
        cos(iTime * 0.4 + p.y * 2.0),
        sin(iTime * 0.6 + p.z * 2.0)
    ) * 0.02;
    p += wobble;

    if (shaderType == 0) {
        // Warm bio-mechanical egg
        float fbmVal = fbm(p * 2.0 + iTime * 0.1);
        float veins = fbm(p * 5.0 + vec3(0.0, iTime * 0.2, 0.0));
        float deepLayer = fbm(p * 4.0 - vec3(iTime * 0.2));
        
        float pulse1 = 0.5 + 0.5 * sin(iTime * 0.8);
        float pulse2 = 0.5 + 0.5 * sin(iTime * 1.2 + 1.0);
        float mainPulse = mix(pulse1, pulse2, 0.5);
        
        vec3 baseColor = mix(
            vec3(0.9, 0.4, 0.2),  // Warmer orange base
            vec3(1.0, 0.6, 0.3),  // Lighter translucent areas
            fbmVal
        );
        
        vec3 veinColor = mix(
            vec3(1.0, 0.2, 0.0),  // Brighter veins
            vec3(0.8, 0.1, 0.0),  // Darker veins
            deepLayer
        ) * (0.8 + 0.2 * mainPulse);
        
        float membrane = fbm(p * 5.0 + vec3(iTime * 0.15));
        float membraneGlow = smoothstep(0.3, 0.7, membrane) * (0.8 + 0.2 * pulse2);
        
        float internalGlow = fbm(p * 6.0 - iTime * 0.1) * mainPulse;
        
        vec3 outColor = mix(baseColor, veinColor, veins * 0.8);
        outColor = mix(outColor, vec3(1.0, 0.6, 0.3), membraneGlow * 0.5);
        outColor += vec3(1.0, 0.4, 0.2) * internalGlow * 0.3;
        outColor += vec3(0.9, 0.3, 0.1) * rim * 0.6;
        
        // Add translucency
        float translucency = pow(1.0 - abs(dot(normal, viewDir)), 2.0);
        outColor += vec3(1.0, 0.5, 0.2) * translucency * 0.4;
        
        outColor *= (0.5 + 0.5 * diff);
        
        float overallPulse = 0.95 + 0.05 * sin(iTime * 0.3);
        finalColor = vec4(outColor, overallPulse);
        
    } else {
        // Cool bio-mechanical egg
        float spiral = fbm(p * 3.0 + vec3(sin(iTime * 0.3)));
        float deepPattern = fbm(p * 4.0 - iTime * 0.2);
        
        float pulse1 = 0.5 + 0.5 * sin(iTime * 0.7);
        float pulse2 = 0.5 + 0.5 * sin(iTime * 1.1);
        
        vec3 baseColor = mix(
            vec3(0.1, 0.4, 0.5),  // Deeper blue
            vec3(0.3, 0.7, 0.9),  // Brighter blue
            deepPattern
        );
        
        // Enhanced cellular pattern
        float cellular = fbm(p * 8.0 + vec3(iTime * 0.1));
        float cellPattern = smoothstep(0.3, 0.7, cellular);
        
        // Energy patterns
        float energyLines = fbm(p * 7.0 + vec3(sin(iTime * 0.4)));
        vec3 energyColor = vec3(0.2, 0.9, 0.8) * (0.7 + 0.3 * pulse1);
        
        float bioLum = fbm(p * 5.0 + vec3(0.0, iTime * 0.2, 0.0));
        vec3 glowColor = vec3(0.1, 0.8, 0.7) * (0.6 + 0.4 * pulse1);
        
        vec3 outColor = mix(baseColor, energyColor, cellPattern * 0.6);
        outColor = mix(outColor, glowColor, bioLum * 0.7);
        outColor += vec3(0.3, 0.9, 1.0) * energyLines * 0.4;
        outColor += vec3(0.2, 0.8, 1.0) * rim * 0.5;
        
        outColor *= (0.6 + 0.4 * diff);
        outColor *= 0.8 + 0.2 * pulse1;
        
        float overallPulse = 0.95 + 0.05 * sin(iTime * 0.3);
        finalColor = vec4(outColor, overallPulse);
    }
}