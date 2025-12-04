#include "TerrainGenerator.h"

int TerrainGenerator::getHeight(int wx, int wz) const {
	float x = wx * params.frequency;
	float z = wz * params.frequency;

	float h = std::sin(x) + std::cos(z * 0.5f);
	h = (h * 0.5f + 0.5f);

	int height = (int)(params.baseHeight + h * params.amplitude);
	return height;
}

BlockType TerrainGenerator::chooseBlock(int wx, int wy, int wz, int groundY) const {
	const int waterLevel = 8;

	if (wy > groundY) {
		if (wy <= waterLevel) return BlockType::Water;
		return BlockType::Air;
	}

	if (groundY == wy) {
		if (groundY <= waterLevel + 1) return BlockType::Sand;
		return BlockType::Grass;
	}

	if (groundY - wy <= 3) {
		return BlockType::Dirt;
	}

	return BlockType::Stone;
}