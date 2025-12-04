#pragma once
#include "../Chunk/Chunk.h"
#include "../Chunk/Block.h"

struct TerrainParams {
	float baseHeight = 8.0f;
	float amplitude = 8.0f;
	float frequency = 0.02f;
};

class TerrainGenerator {
public:
	TerrainGenerator(const TerrainParams& params = TerrainParams()) : params(params) {};

	// wx wz -> wy
	int getHeight(int wx, int wz) const;

	BlockType chooseBlock(int wx, int wy, int wz, int groundY) const;

private:
	TerrainParams params;
};