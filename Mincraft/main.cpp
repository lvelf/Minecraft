#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Camera/Camera.h"
#include "callbacks.h"
#include "Chunk/ChunkManager.h"
#include "MyShader.h"



int windowLength = 1200;
int windowHeight = 1200;

static ChunkManager chunkManager;
static bool worldInitialized = false;
void CreateExampleWorld();
void LoadTexture();

// Texture
GLuint gStoneTex = 0;

void Init() {
	glEnable(GL_DEPTH_TEST);
	LoadTexture();
	CreateExampleWorld();

	worldInitialized = true;
}

void CreateExampleWorld() {
	int minX = -16;
	int maxX = 16;
	int minZ = -16;
	int maxZ = 16;

	for (int x = minX; x < maxX; ++x) {
		for (int z = minZ; z < maxZ; ++z) {
			int groundY = 0;
			

			// y < -3: Stone
			for (int y = -8; y < -3; ++y) {
				chunkManager.setBlockWorld(x, y, z, BlockType::Stone);
			}

			// -3 <= y < -1: Dirt
			for (int y = -3; y < -1; ++y) {
				chunkManager.setBlockWorld(x, y, z, BlockType::Dirt);
			}

			chunkManager.setBlockWorld(x, -1, z, BlockType::Grass);

		}
	}


	for (int x = -5; x <= 5; ++x) {
		for (int z = -5; z <= 5; ++z) {
			int height = 1 + (std::abs(x) + std::abs(z)) % 4; 
			for (int h = 0; h < height; ++h) {
				chunkManager.setBlockWorld(x, -1 + h, z, BlockType::Dirt);
			}
			
			chunkManager.setBlockWorld(x, -1 + height, z, BlockType::Grass);
		}
	}
}

void LoadTexture() {
	int w, h, channels;
	unsigned char* data = SOIL_load_image("../data/stone.png",
		&w, &h, &channels,
		SOIL_LOAD_RGBA);

	if (!data) {
		std::cerr << "Failed to load ./data/stone.png\n";
		return;
	}

	// TODO: texture mapping
	glGenTextures(1, &gStoneTex);
	glBindTexture(GL_TEXTURE_2D, gStoneTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);


	// Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	SOIL_free_image_data(data);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void display(int width, int height) {
	static MyShader blockShader("./Shaders/Block.vert", "./Shaders/Block.frag");
	
	if (!worldInitialized) {
		CreateExampleWorld();
		worldInitialized = true;
	}

	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//default setting for camera
	//glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 10.0f); 
	//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); 

	glm::vec3 cameraPos = glm::vec3(0.0f, 25.0f, 40.0f);
	//glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 20.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, -5.0f, 0.0f); 
	//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); 

	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

	// projection
	float fov = glm::radians(60.0f);
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	glm::mat4 projection = glm::perspective(fov, aspect, nearPlane, farPlane);

	

	blockShader.Use();
	blockShader.setMat4("uView", view);
	blockShader.setMat4("uProjection", projection);
	
	//texture + light
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gStoneTex);
	blockShader.setInt("uDiffuseTex", 0);
	blockShader.setVec3("uLightDir", glm::vec3(-1.0f, -1.0f, -0.3f));

	chunkManager.renderAll();

}

int main() {

	std::cout << "begin initial" << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window
	GLFWwindow* window = glfwCreateWindow(windowLength, windowHeight, "Minecraft", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Could not create the window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// make context
	glfwMakeContextCurrent(window);

	// associate callbacks
	//----------------------------------------------
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	
	// set input mode
	// ------------------------------------------
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Init GLEW
	// ------------------------------------
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not initialized." << std::endl;
		return -1;
	}

	// OpenGL Window and viewport parameter setup
	int widthBuff, heightBuff;
	glfwGetFramebufferSize(window, &widthBuff, &heightBuff);
	glViewport(0, 0, widthBuff, heightBuff);

	// Init
	Init();

	// loop and display as long as the window is open
	while (!glfwWindowShouldClose(window)) {
		
		// draw
		display(widthBuff, heightBuff);

		// swap buffers
		glfwSwapBuffers(window);

		// io and callbacks
		glfwPollEvents();

	}

	//--------------------------------------------

	// cleanup window
	glfwDestroyWindow(window);

	// end process
	glfwTerminate();

	// return

	return 0;
}