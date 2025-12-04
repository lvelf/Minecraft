#pragma once
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <cstddef>
#include "../MyShader.h"

class Sky {
public:
	MyShader* gSkyShader;
	std::array<float, 108> skyboxVertices;
    GLuint gSkyVAO = 0, gSkyVBO = 0;


    Sky();

    ~Sky();

    void render();

};