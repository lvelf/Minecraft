#version 330 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;
layout(location=3) in vec3 aTangent;
layout(location=4) in vec3 aBiTangent;


uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
out vec3 vTangent;
out vec3 vBiTangent;

void main() {
	vWorldPos = aPos;
	vNormal = normalize(aNormal);
	vUV = aUV;
	vTangent = normalize(aTangent);
	vBiTangent = normalize(aBiTangent);
	gl_Position = uProjection * uView * vec4(aPos, 1.0);
}