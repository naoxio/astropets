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
vec3 getBaseColor(int colorType, float variation) {
    // Base colors with variation
    switch(colorType) {
        case 0: // Red shades
            return mix(
                vec3(0.9, 0.1, 0.1),
                vec3(1.0, 0.3, 0.3),
                variation
            );
        case 1: // Green shades
            return mix(
                vec3(0.2, 0.8, 0.2),
                vec3(0.4, 0.9, 0.4),
                variation
            );
        case 2: // Blue shades
            return mix(
                vec3(0.1, 0.4, 0.9),
                vec3(0.3, 0.6, 1.0),
                variation
            );
        case 3: // Purple shades
            return mix(
                vec3(0.5, 0.0, 0.8),
                vec3(0.7, 0.2, 1.0),
                variation
            );
        case 4: // Pink shades
            return mix(
                vec3(0.9, 0.2, 0.6),
                vec3(1.0, 0.4, 0.8),
                variation
            );
        case 5: // Orange shades
            return mix(
                vec3(0.9, 0.4, 0.1),
                vec3(1.0, 0.6, 0.2),
                variation
            );
        case 6: // Cyan shades
            return mix(
                vec3(0.0, 0.8, 0.8),
                vec3(0.2, 0.9, 0.9),
                variation
            );
        case 7: // Yellow shades
            return mix(
                vec3(0.9, 0.8, 0.1),
                vec3(1.0, 0.9, 0.2),
                variation
            );
        case 8: // Brown shades
            return mix(
                vec3(0.6, 0.3, 0.1),
                vec3(0.7, 0.4, 0.2),
                variation
            );
        default: // Default red
            return vec3(0.9, 0.1, 0.1);
    }
}

vec3 getAccentColor(int colorType, float variation) {
    // Accent colors that complement the base colors
    switch(colorType) {
        case 0: // Red accents
            return mix(
                vec3(1.0, 0.2, 0.2),
                vec3(1.0, 0.4, 0.4),
                variation
            );
        case 1: // Green accents
            return mix(
                vec3(0.3, 1.0, 0.3),
                vec3(0.5, 1.0, 0.5),
                variation
            );
        case 2: // Blue accents
            return mix(
                vec3(0.2, 0.6, 1.0),
                vec3(0.4, 0.8, 1.0),
                variation
            );
        case 3: // Purple accents
            return mix(
                vec3(0.8, 0.2, 1.0),
                vec3(1.0, 0.4, 1.0),
                variation
            );
        case 4: // Pink accents
            return mix(
                vec3(1.0, 0.3, 0.7),
                vec3(1.0, 0.5, 0.9),
                variation
            );
        case 5: // Orange accents
            return mix(
                vec3(1.0, 0.5, 0.0),
                vec3(1.0, 0.7, 0.2),
                variation
            );
        case 6: // Cyan accents
            return mix(
                vec3(0.0, 1.0, 1.0),
                vec3(0.3, 1.0, 1.0),
                variation
            );
        case 7: // Yellow accents
            return mix(
                vec3(1.0, 0.9, 0.0),
                vec3(1.0, 1.0, 0.3),
                variation
            );
        case 8: // Brown accents
            return mix(
                vec3(0.8, 0.4, 0.1),
                vec3(0.9, 0.5, 0.2),
                variation
            );
        default: // Default red accent
            return vec3(1.0, 0.2, 0.2);
    }
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

    vec3 p = fragPosition * 20.0;
    vec3 wobble = vec3(
        sin(iTime * 0.5 + p.x * 2.0),
        cos(iTime * 0.4 + p.y * 2.0),
        sin(iTime * 0.6 + p.z * 2.0)
    ) * 0.02;
    p += wobble;

    // Get variation based on noise
    float variation = fbm(p * 2.0 + iTime * 0.1);
    
    // Get base and accent colors
    vec3 baseColor = getBaseColor(shaderType, variation);
    vec3 accentColor = getAccentColor(shaderType, variation);

    float spiral = fbm(p * 3.0 + vec3(sin(iTime * 0.3)));
    float deepPattern = fbm(p * 4.0 - iTime * 0.2);
    float pulse1 = 0.5 + 0.5 * sin(iTime * 0.7);
    float pulse2 = 0.5 + 0.5 * sin(iTime * 1.1);

    // Enhanced cellular pattern
    float cellular = fbm(p * 8.0 + vec3(iTime * 0.1));
    float cellPattern = smoothstep(0.3, 0.7, cellular);

    // Energy patterns
    float energyLines = fbm(p * 7.0 + vec3(sin(iTime * 0.4)));
    float bioLum = fbm(p * 5.0 + vec3(0.0, iTime * 0.2, 0.0));

    vec3 outColor = mix(baseColor, accentColor, cellPattern * 0.6);
    outColor = mix(outColor, accentColor * 1.2, bioLum * 0.4);
    outColor += accentColor * energyLines * 0.3;
    outColor += accentColor * rim * 0.5;

    outColor *= (0.6 + 0.4 * diff);
    outColor *= 0.8 + 0.2 * pulse1;

    float overallPulse = 0.95 + 0.05 * sin(iTime * 0.3);
    finalColor = vec4(outColor, overallPulse);
}
