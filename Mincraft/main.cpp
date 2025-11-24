#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Camera.h"
#include "callbacks.h"


int windowLength = 1200;
int windowHeight = 1200;

void Init() {

}

void display(int width, int length) {

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