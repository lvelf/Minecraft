#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

uniform sampler2D uDiffuseTex;
uniform vec3 uLightDir; 

out vec4 FragColor;

void main() {
	
	vec3 albedo = texture(uDiffuseTex, vUV).rgb;

	vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir);
    float NdotL = max(dot(N, L), 0.0);
    vec3 color = albedo * (0.1 + 0.9 * NdotL);

    FragColor = vec4(color, 1.0);
}