#version 330 core

in vec3 vWorldPos;

out vec4 FragColor;

void main() {
	if (vWorldPos.y < -3.5) {
         FragColor = vec4(0.5, 0.5, 0.5, 1.0);   
    }   
    else if (vWorldPos.y < -1.5) {
        FragColor = vec4(0.4, 0.25, 0.1, 1.0);  
    }      
    else {
         FragColor = vec4(0.2, 0.7, 0.2, 1.0);      
    }
}