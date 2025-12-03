#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

uniform sampler2D uDiffuseTex;
uniform vec3 uLightDir; 
uniform float uTime;

out vec4 FragColor;

void main() {
    vec2 uv = vUV;

	
    // water running

    const float cellX = 1.0 / 4.0;
    const float cellY = 1.0 / 2.0;


    vec2 waterMin = vec2(2.0 * cellX, 0.0 * cellY);              // (0.5, 0.0)
    vec2 waterSize = vec2(cellX, cellY);                         // (0.25, 0.5)
    vec2 waterMax = waterMin + waterSize;

    
    if (uv.x >= waterMin.x && uv.x <= waterMax.x &&
        uv.y >= waterMin.y && uv.y <= waterMax.y)
    {
        
        vec2 localUV = (uv - waterMin) / waterSize;

       
        localUV.x += uTime * 0.1;
        localUV.y += uTime * 0.05;
        localUV = fract(localUV); 

        
        const float eps = 0.001;
        vec2 innerSize = waterSize * (1.0 - 2.0 * eps);
        uv = waterMin + waterSize * eps + localUV * innerSize;
    }
    vec3 albedo = texture(uDiffuseTex, uv).rgb;
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir);
    float NdotL = max(dot(N, L), 0.0);
    vec3 color = albedo * (0.1 + 0.9 * NdotL);



    FragColor = vec4(color, 1.0);
}