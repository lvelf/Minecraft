#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
in vec3 vTangent;
in vec3 vBiTangent;

uniform sampler2D uDiffuseTex;
uniform sampler2D uNormalMap;
uniform vec3 uLightDir; 
uniform float uTime;
uniform vec3 uCameraPos;

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

    // normal mapping 
    vec3 mapN = texture(uNormalMap, uv).xyz * 2.0f - 1.0f;
    vec3 N = normalize(vNormal);
    vec3 T = normalize(vTangent);
    vec3 B = normalize(vBiTangent);
    mat3 TBN = mat3(T, B, N);
    // normal 
    
    vec3 norm = normalize(TBN * mapN);

    //vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(-uLightDir);
    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    vec3 H = normalize(lightDir + viewDir);

    float NoL = max(dot(norm, lightDir), 0.0);
    float NoV = max(dot(norm, viewDir),  0.0);
    float NoH = max(dot(norm, H),        0.0);
    float VoH = max(dot(viewDir, H),     0.0);

    NoH = clamp(NoH, 0.05, 1.0);

    vec3 albedo = texture(uDiffuseTex, uv).rgb;
    albedo = pow(albedo, vec3(2.2)); // albeo

    vec3 ambient = vec3(0.3);
    vec3 diffuse = NoL * vec3(1.0);

    // Fresnel D G
    float F0 = 0.04;                                
    float F  = F0 + (1.0 - F0) * pow(1.0 - VoH, 5.0); 

    // Microfacet Distribution: Beckmann
    float m  = 0.3;            
    float m2 = m * m;
    float NoH2 = NoH * NoH;
    float NoH4 = NoH2 * NoH2;

     float denomNH = max(NoH2, 1e-4);
    float tanThetaH2 = (1.0 - NoH2) / denomNH;
    float D = exp(-tanThetaH2 / m2) / (m2 * max(NoH4, 1e-4));

     // Geometry term G
    float denomVoH = max(VoH, 1e-4);
    float k1 = (2.0 * NoH * NoV) / denomVoH;
    float k2 = (2.0 * NoH * NoL) / denomVoH;
    float G = min(1.0, min(k1, k2));


    // specular
    float denomSpec = max(3.14159265 * NoV * NoL, 1e-4);
    float specularScalar = (F * D * G) / denomSpec;
    vec3 specular = specularScalar * vec3(1.0);

    specular = clamp(specular, 0.0f, 1.5f);

    vec3 color = albedo * (ambient + diffuse)
           + specular * albedo;


    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}