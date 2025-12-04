#version 330 core

in vec3 vDir;
out vec4 FragColor;

uniform samplerCube uSkyTex;

void main()
{
    vec3 dir = normalize(vDir);
    vec3 col = texture(uSkyTex, dir).rgb;
    FragColor = vec4(col, 1.0);
}