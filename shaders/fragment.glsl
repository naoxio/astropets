#version 330 core
out vec4 FragColor;

uniform vec2 iResolution;
uniform float iTime;
uniform vec3 cameraPos;
uniform vec3 cameraFront;
uniform vec3 cameraUp;
uniform vec3 color;
uniform int shaderType;

float hash(float n) { return fract(sin(n) * 753.5453123); }

float noise(vec3 p) {
    vec3 fp = floor(p);
    vec3 fr = fract(p);
    fr = fr * fr * (3.0 - 2.0 * fr);
    float n = fp.x + fp.y * 157.0 + 113.0 * fp.z;
    return mix(mix(
        mix(hash(n + 0.0), hash(n + 1.0), fr.x),
        mix(hash(n + 157.0), hash(n + 158.0), fr.x), fr.y),
        mix(mix(hash(n + 113.0), hash(n + 114.0), fr.x),
        mix(hash(n + 270.0), hash(n + 271.0), fr.x), fr.y), fr.z);
}

float fbm(vec3 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 3; i++) { // Reduced from 5 to 3
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main() {
    if (color != vec3(0.0)) {
        FragColor = vec4(color, 1.0);
        return;
    }

    vec2 uv = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.y;
    vec3 ro = cameraPos;
    vec3 rd = normalize(vec3(uv, 1.0));
    vec3 cameraRight = normalize(cross(cameraFront, cameraUp));
    vec3 cameraUpAdjusted = normalize(cross(cameraRight, cameraFront));
    rd = normalize(rd.x * cameraRight + rd.y * cameraUpAdjusted + rd.z * cameraFront);

    float a = 0.8;
    float b = 1.5;
    mediump float t = 0.0;
    float minStep = 0.001;
    float maxStep = 10.0;
    float threshold = 0.001;
    
    for(int i = 0; i < 100; i++) { // Reduced from 200 to 100
        vec3 p = ro + rd * t;
        float x = p.x / a;
        float y = (p.y + 0.2) / b;
        float z = p.z / a;
        
        float deform = shaderType == 0 ? 0.1 * fbm(p * 5.0 + iTime * 0.1) : 0.15 * fbm(p * 3.0 + iTime * 0.1);
        float d = length(vec3(x, y, z)) - (1.0 + deform);
        
        if(abs(d) < threshold || t > maxStep) break; // Early exit
        t += d * 0.5; // Fixed step size
    }
    
    vec3 p = ro + rd * t;
    vec3 n = normalize(p);
    vec3 l = normalize(vec3(1.0, 1.0, -1.0));
    float diff = max(dot(n, l), 0.0);

    float pulse = 0.5 + 0.5 * sin(iTime * 0.5);
    
    if (shaderType == 0) {
        float fbmVal = fbm(p * 3.0); // Cached fbm value
        float veins = fbm(p * 15.0 + vec3(0.0, iTime * 0.1, 0.0));
        float veinsPattern = smoothstep(0.4, 0.6, veins);
        
        vec3 baseColor = mix(
            vec3(0.8, 0.2, 0.0),
            vec3(1.0, 0.5, 0.0),
            fbmVal
        );
        
        vec3 veinColor = vec3(1.0, 0.3, 0.0) * (0.8 + 0.2 * pulse);
        
        float spots = fbm(p * 20.0);
        float spotsPattern = smoothstep(0.5, 0.7, spots);
        vec3 spotColor = vec3(1.0, 0.8, 0.0);
        
        vec3 finalColor = mix(baseColor, veinColor, veinsPattern);
        finalColor = mix(finalColor, spotColor, spotsPattern * 0.5);
        
        float glow = (1.0 - abs(dot(n, rd))) * (1.0 - abs(dot(n, rd))); // Cheaper than pow
        finalColor += vec3(1.0, 0.5, 0.0) * glow * 0.3;
        
        FragColor = vec4(finalColor, 1.0);
    } else {
        float spiral = fbm(p * 10.0 + vec3(sin(iTime * 0.2) * 0.5));
        float spiralPattern = smoothstep(0.3, 0.7, spiral);
        
        vec3 baseColor = mix(
            vec3(0.2, 0.0, 0.4),
            vec3(0.4, 0.0, 0.8),
            fbm(p * 4.0)
        );
        
        float bioLum = fbm(p * 25.0 + vec3(0.0, iTime * 0.15, 0.0));
        float bioPattern = smoothstep(0.6, 0.8, bioLum);
        vec3 glowColor = vec3(0.0, 0.8, 1.0) * (0.8 + 0.2 * pulse);
        
        vec3 finalColor = mix(baseColor, glowColor, bioPattern);
        finalColor = mix(finalColor, vec3(0.6, 0.2, 1.0), spiralPattern * 0.4);
        
        float glow = (1.0 - abs(dot(n, rd))) * (1.0 - abs(dot(n, rd))); // Cheaper than pow
        finalColor += vec3(0.2, 0.4, 1.0) * glow * 0.2;
        
        FragColor = vec4(finalColor, 1.0);
    }
}