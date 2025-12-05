#pragma once
#include <glm/glm.hpp>
#include "../Chunk/ChunkManager.h"

class Player {
public:
	Player(const glm::vec3& startPos) : position(startPos), velocity(0.0f) {
		yaw = 0.0f;
		pitch = -20.0f;
	}

	void update(float deltaTime, ChunkManager& chunkManager);
	void processInput(GLFWwindow* window, float deltaTime, ChunkManager& chunkManager);

	glm::vec3 getPosition() const { return position; }
	glm::vec3 getCameraPosition() const {
		return position + glm::vec3(0.0f, 1.6f, 0.0f);
	}
	glm::vec3 getForward() const;
	glm::vec3 getRight() const;

	glm::vec3 getCameraTarget() const {
		return getCameraPosition() + getForward();
	}

	glm::vec3 getCameraUp() const {
		return glm::vec3(0.0f, 1.0f, 0.0f);
	}

	glm::mat4 getViewMatrix() const {
		return glm::lookAt(getCameraPosition(), getCameraTarget(), getCameraUp());
	}

	float getYaw() const { return yaw; }
	float getPitch() const { return pitch; }

	void setYaw(float y) { yaw = y; }
	void setPitch(float p) {
		pitch = glm::clamp(p, -89.0f, 89.0f);
	}

	void jump();

private:

	glm::vec3 position;
	glm::vec3 velocity;

	// rotation
	float yaw; // horizontal
	float pitch; // vertical

	const float GRAVITY = -20.0f;
	const float MOVE_SPEED = 5.0f;
	const float PLAYER_HEIGHT = 1.8f;
	const float PLAYER_RADIUS = 0.3f;

	bool isJumping = false;
	const float JUMP_VELOCITY = 8.0f;
	
	bool isOnGround(ChunkManager& chunkManager);
	bool checkCollision(const glm::vec3& newPos, ChunkManager& chunkManager);
	float getGroundHeight(const glm::vec3& pos, ChunkManager& chunkManager);

	bool blockNotAir(Block& block) const { return block.type != BlockType::Air && block.type != BlockType::Water; }
};