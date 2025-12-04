
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
#include "Sky/Sky.h"
#include <stb_image.h>
#include "World/WorldManager.h"

int windowLength = 1600;
int windowHeight = 1600;

static ChunkManager chunkManager;
static std::unique_ptr<WorldManager> worldManager;
static bool worldInitialized = false;
void CreateExampleWorld();
void CreateWorld();
void LoadTexture();

// Texture
GLuint gStoneTex = 0;
GLuint gNormalTex = 0;
GLuint gSkyTex = 0;

static Sky* sky;

// Camera

// World
int worldRadius = 30;
int worldHeight = 20;



void Init() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LoadTexture();
	//CreateExampleWorld();
	
	worldManager = std::make_unique<WorldManager>(worldRadius, worldHeight);
	CreateWorld();
	sky = new Sky();
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
			
			// y < -5: Stone
			for (int y = -8; y < -5; ++y) {
				chunkManager.setBlockWorld(x, y, z, BlockType::Sand);
			}

			// y < -3: Stone
			for (int y = -5; y < -3; ++y) {
				chunkManager.setBlockWorld(x, y, z, BlockType::Stone);
			}

			// -3 <= y < -1: Dirt
			for (int y = -3; y < -1; ++y) {
				chunkManager.setBlockWorld(x, y, z, BlockType::Dirt);
			}

			chunkManager.setBlockWorld(x, -1, z, BlockType::Water);

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

void CreateWorld() {
	worldManager->generateInitialWolrd();
}


void LoadCrossCubemap(const char* path)
{
	int width, height, channels;
	unsigned char* data = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGB);
	if (!data) {
		std::cerr << "Failed to load cubemap cross: " << path << "\n";
		return;
	}

	std::cout << "Loaded cubemap: " << width << "x" << height << std::endl;
	int faceSize = width / 4;   // 512 / 4 = 128



	glGenTextures(1, &gSkyTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyTex);


	std::vector<unsigned char> face(faceSize * faceSize * 3);

	auto copyFace = [&](int faceIndex, int startX, int startY)
		{

			for (int y = 0; y < faceSize; ++y) {
				for (int x = 0; x < faceSize; ++x) {
					int srcX = startX + x;
					int srcY = startY + y;

					int srcIndex = (srcY * width + srcX) * 3;
					int destIndex = (y * faceSize + x) * 3;

					face[destIndex + 0] = data[srcIndex + 0];
					face[destIndex + 1] = data[srcIndex + 1];
					face[destIndex + 2] = data[srcIndex + 2];
				}
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
				0, GL_RGB,
				faceSize, faceSize,
				0, GL_RGB, GL_UNSIGNED_BYTE, face.data());
		};


	// +X -X +Y -Y +Z -Z
	copyFace(0, 2 * faceSize, 1 * faceSize);
	copyFace(1, 0 * faceSize, 1 * faceSize);
	copyFace(2, 1 * faceSize, 0 * faceSize);
	copyFace(3, 1 * faceSize, 2 * faceSize);
	copyFace(4, 1 * faceSize, 1 * faceSize);
	copyFace(5, 3 * faceSize, 1 * faceSize);

	SOIL_free_image_data(data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void LoadTexture() {
	int w, h, channels;
	unsigned char* data = SOIL_load_image("../data/blocks.png",
		&w, &h, &channels,
		SOIL_LOAD_RGBA);

	if (!data) {
		std::cerr << "Failed to load ../data/blocks.png\n";
		return;
	}

	// TODO: texture mapping
	glGenTextures(1, &gStoneTex);
	glBindTexture(GL_TEXTURE_2D, gStoneTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);


	// Filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Wrapping
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SOIL_free_image_data(data);
	glBindTexture(GL_TEXTURE_2D, 0);


	// normal map
	data = SOIL_load_image("../data/blocks_normal.png",
		&w, &h, &channels,
		SOIL_LOAD_RGBA);
	if (!data) {
		std::cerr << "Failed to load blocks_normal png" << std::endl;
		return;
	}

	glGenTextures(1, &gNormalTex);
	glBindTexture(GL_TEXTURE_2D, gNormalTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Wrapping

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SOIL_free_image_data(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	// sky dome
	/*
	data = SOIL_load_image("../data/sky.png", &w, &h, &channels, SOIL_LOAD_RGB);
	if (!data) {
		std::cerr << "Failed to load sky dome png" << std::endl;
		std::cerr << "SOIL error: " << SOIL_last_result() << std::endl;
		return;
	}

	glGenTextures(1, &gSkyTex);
	glBindTexture(GL_TEXTURE_2D, gSkyTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SOIL_free_image_data(data);*/
	/*
	unsigned char* sky_data = stbi_load("../data/sky.png", &w, &h, &channels, 3);
	if (!sky_data) {
		std::cerr << "Failed to load sky.png: " << stbi_failure_reason() << std::endl;
		return;
	}

	glGenTextures(1, &gSkyTex);
	glBindTexture(GL_TEXTURE_2D, gSkyTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, sky_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(sky_data);*/
	LoadCrossCubemap("../data/cubemap.png");
	glBindTexture(GL_TEXTURE_2D, 0);
}



void display(int width, int height) {
	static MyShader blockShader("./Shaders/Block.vert", "./Shaders/Block.frag");
	static MyShader gSkyShader("./Shaders/Sky.vert", "./Shaders/Sky.frag");
	static float time = 0.0f;
	time += 0.016f;

	if (!worldInitialized) {
		CreateExampleWorld();
		worldInitialized = true;
	}

	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//default setting for camera
	//glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 10.0f); 
	//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); 

	glm::vec3 cameraPos = glm::vec3(0.0f, 75.0f, 70.0f);
	//glm::vec3 cameraPos = glm::vec3(10.0f, 5.0f, 20.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, -5.0f, 0.0f); 
	//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); 

	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
	// camera 
	//glm::mat4 view = camera.GetViewMatrix();
	// projection
	float fov = glm::radians(60.0f);
	//float fov = glm::radians(camera.Zoom);


	float aspect = static_cast<float>(width) / static_cast<float>(height);
	float nearPlane = 0.1f;
	float farPlane = 1000.0;
	glm::mat4 projection = glm::perspective(fov, aspect, nearPlane, farPlane);

	// Sun
	glm::vec3 sunDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.3f));
	
	// Draw Sky firstly
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);
	gSkyShader.Use();

	glm::mat4 viewRot = glm::mat4(glm::mat3(view));
	gSkyShader.setMat4("uProjection", projection);
	gSkyShader.setMat4("uView", viewRot);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyTex);
	gSkyShader.setInt("uSkyTex", 2);
	sky->render();
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	// ==============================================


	blockShader.Use();
	blockShader.setMat4("uView", view);
	blockShader.setMat4("uProjection", projection);
	
	//texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gStoneTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormalTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyTex);

	blockShader.setInt("uDiffuseTex", 0);
	blockShader.setInt("uNormalMap", 1);
	blockShader.setInt("uCubeMap", 2);
	//light
	blockShader.setVec3("uLightDir", sunDir);
	blockShader.setFloat("uTime", time);
	blockShader.setVec3("uCameraPos", cameraPos);

	//chunkManager.renderAll();
	worldManager->render();
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
	delete sky;
	sky = nullptr;
	// cleanup window
	glfwDestroyWindow(window);

	// end process
	glfwTerminate();

	// return

	return 0;
}