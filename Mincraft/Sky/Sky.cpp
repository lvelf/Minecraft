#include "Sky.h"

Sky::Sky() {

    float size = 5000.0f;
    skyboxVertices = {
       -size,  size, -size,
       -size, -size, -size,
        size, -size, -size,
        size, -size, -size,
        size,  size, -size,
       -size,  size, -size,

       -size, -size,  size,
       -size, -size, -size,
       -size,  size, -size,
       -size,  size, -size,
       -size,  size,  size,
       -size, -size,  size,

        size, -size, -size,
        size, -size,  size,
        size,  size,  size,
        size,  size,  size,
        size,  size, -size,
        size, -size, -size,

       -size, -size,  size,
       -size,  size,  size,
        size,  size,  size,
        size,  size,  size,
        size, -size,  size,
       -size, -size,  size,

       -size,  size, -size,
        size,  size, -size,
        size,  size,  size,
        size,  size,  size,
       -size,  size,  size,
       -size,  size, -size,

       -size, -size, -size,
       -size, -size,  size,
        size, -size, -size,
        size, -size, -size,
       -size, -size,  size,
        size, -size,  size
    };

    glGenVertexArrays(1, &gSkyVAO);
    glGenBuffers(1, &gSkyVBO);

    glBindVertexArray(gSkyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gSkyVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), skyboxVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    glBindVertexArray(0);
    //gSkyShader = new MyShader("../Shaders/Sky.vert", "../Shaders/Sky.frag");
}

Sky::~Sky() {
    glDeleteVertexArrays(1, &gSkyVAO);
    glDeleteBuffers(1, &gSkyVBO);

    if (gSkyShader != nullptr) {
        delete gSkyShader;
        gSkyShader = nullptr;
    }
}

void Sky::render() {
    glBindVertexArray(gSkyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

