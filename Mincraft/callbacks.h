#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "./Camera/Camera.h"
#include <memory>
#include "Player/Player.h"

struct AppState {
	Player* player;
	bool firstMouse = true;
	float lastMouseX = 800.0f;
	float lastMouseY = 600.0f;
};

void KeyCallback(GLFWwindow* window, int key, int scan, int act, int mode);
void MouseCallback(GLFWwindow* window, double xPosition, double yPosition);