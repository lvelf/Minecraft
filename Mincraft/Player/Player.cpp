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

	// dig block
	HitInfo currentHit = raycastBlock(getCameraPosition(), getForward(), 6.0f, chunkManager);

	int leftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int rightState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	if (rightState == GLFW_PRESS) {
		std::cout << "current hit status: " << "hit - " << currentHit.hit << "hit place: " << currentHit.placePos.x << " " << currentHit.placePos.y << " " << currentHit.placePos.z << " Block Type is:" <<  blockName(Block(currentHit.type) ) << std::endl;

		if (!prevRightPressed) {
			if (hasHeldBlock && currentHit.hit) {
				glm::ivec3 p = currentHit.placePos;
				Block b = chunkManager.getBlockWorld(p.x, p.y, p.z);
				if (b.type == BlockType::Air) {
					chunkManager.setBlockWorld(p.x, p.y, p.z, heldBlock);
				}
			}
		}
		prevRightPressed = true;
	}
	else {
		prevRightPressed = false;
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


HitInfo Player::raycastBlock(const glm::vec3& origin,
	const glm::vec3& dir,
	float maxDist,
	ChunkManager& cm) {
	
	HitInfo info;
	glm::vec3 pos = origin;
	glm::vec3 step = glm::normalize(dir) * 0.1f;

	float dist = 0.0f;
	glm::ivec3 lastBlockPos((int)std::floor(origin.x),
		(int)std::floor(origin.y),
		(int)std::floor(origin.z));

	while (dist < maxDist) {
		pos += step;
		dist += 0.1f;

		glm::ivec3 bpos((int)std::floor(pos.x),
			(int)std::floor(pos.y),
			(int)std::floor(pos.z));

		if (bpos != lastBlockPos) {
			Block b = cm.getBlockWorld(bpos.x, bpos.y, bpos.z);
			if (b.type != BlockType::Air) {
				info.hit = true;
				info.bx = bpos.x;
				info.by = bpos.y;
				info.bz = bpos.z;
				info.type = b.type;

				glm::vec3 diff = pos - glm::vec3(bpos) - glm::vec3(0.5f);
				glm::ivec3 n(0);
				if (std::abs(diff.x) > std::abs(diff.y) &&
					std::abs(diff.x) > std::abs(diff.z))
					n.x = (diff.x > 0) ? 1 : -1;
				else if (std::abs(diff.y) > std::abs(diff.z))
					n.y = (diff.y > 0) ? 1 : -1;
				else
					n.z = (diff.z > 0) ? 1 : -1;
				info.hitNormal = n;
				info.placePos = glm::ivec3(info.bx, info.by, info.bz) + n;
				return info;
			}
			lastBlockPos = bpos;
		}
	}

	return info;
}