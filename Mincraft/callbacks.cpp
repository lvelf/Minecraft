#include "callbacks.h"
#include <iostream>



void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));

	if (!state || !state->player)
		return;

	if (state->firstMouse) {
		state->lastMouseX = xpos;
		state->lastMouseY = ypos;
		state->firstMouse = false;
	}

	float xoffset = xpos - state->lastMouseX;
	float yoffset = state->lastMouseY - ypos;
	state->lastMouseX = xpos;
	state->lastMouseY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	state->player->setYaw(state->player->getYaw() + xoffset);
	state->player->setPitch(state->player->getPitch() + yoffset);

}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}