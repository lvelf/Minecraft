#include "Player.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>


glm::vec3 Player::getForward() const {
	glm::vec3 forward;

	forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward.y = sin(glm::radians(pitch));
	forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	return glm::normalize(forward);
}

glm::vec3 Player::getRight() const {
	glm::vec3 forward = getForward();
	return glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
}

bool Player::isOnGround(ChunkManager& chunkManager) {
	int px = (int)std::floor(position.x);
	int pyBelow = (int)std::floor(position.y - 0.1f);
	int pz = (int)std::floor(position.z);


	Block block = chunkManager.getBlockWorld(px, pyBelow, pz);
	
	//std::cout << "now your bottom block is:" << blockName(block) << std::endl;

	

	return (block.type != BlockType::Air && block.type != BlockType::Water);
}

float Player::getGroundHeight(const glm::vec3& pos, ChunkManager& chunkManager) {
	int px = (int)std::floor(position.x);
	int pz = (int)std::floor(position.z);

	for (int y = (int)pos.y; y > (int)pos.y - 5; --y) {
		Block block = chunkManager.getBlockWorld(px, y, pz);
		if (block.type == BlockType::Water) {
			return pos.y;
		}
		if (block.type != BlockType::Air) {
			return (float)y + 1;
		}
	}
	return pos.y;
}

bool Player::checkCollision(const glm::vec3& newPos, ChunkManager& chunkManager) {
	int px = (int)std::floor(newPos.x);
	int pz = (int)std::floor(newPos.z);

	for (int dy = 0; dy < 2; dy++) {
		int py = (int)std::floor(newPos.y) + dy;
		Block block = chunkManager.getBlockWorld(px, py, pz);
		if (blockNotAir(block)) {
			return true;
		}
	}
	return false;
}

void Player::processInput(GLFWwindow* window, float deltaTime, ChunkManager& chunkManager) {
	glm::vec3 forward = getForward();
	glm::vec3 right = getRight();

	forward.y = 0.0f;
	right.y = 0.0f;
	forward = glm::normalize(forward);
	right = glm::normalize(right);

	glm::vec3 moveDir(0.0f);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		moveDir += forward;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		moveDir -= forward;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		moveDir += right;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		moveDir -= right;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		if (isOnGround(chunkManager)) {
			jump();
		}
	}

	if (glm::length(moveDir) > 0.01f) {
		moveDir = glm::normalize(moveDir);
		glm::vec3 newPos = position + moveDir * MOVE_SPEED * deltaTime;

		if (!checkCollision(newPos, chunkManager)) {
			position.x = newPos.x;
			position.z = newPos.z;
		}
		
	}

}

void Player::update(float deltaTime, ChunkManager& chunkManager) {
	
	if (!isOnGround(chunkManager)) {
		velocity.y += GRAVITY * deltaTime;
	}
	else {
		if (velocity.y <= 0.0f) {
			velocity.y = 0.0f;
			float groundY = getGroundHeight(position, chunkManager);
			position.y = groundY;
			isJumping = false;
		}
	}

	position.y += velocity.y * deltaTime;

	if (position.y < -20.f) {
		position.y = 20.0f;
		velocity.y = 0.0f;
		isJumping = false;
	}
}

void Player::jump() {
	if (!isJumping) {
		velocity.y = JUMP_VELOCITY;
		isJumping = true;
	}
}