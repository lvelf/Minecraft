#pragma once
#include "../Chunk/Chunk.h"
#include "../Chunk/Block.h"
#include "FastNoiseLite.h"

struct TerrainParams {
	float baseHeight = 5.0f;
	float amplitude = 8.0f;
	float frequency = 0.02f;
};

class TerrainGenerator {
public:
	TerrainGenerator(const TerrainParams& params = TerrainParams(), int seed = 1337) : params(params), seed(seed) {
		noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise.SetSeed(seed);
		noise.SetFrequency(params.frequency);
	};

	// wx wz -> wy
	int getHeight(int wx, int wz) const;

	BlockType chooseBlock(int wx, int wy, int wz, int groundY) const;

private:
	TerrainParams params;
	FastNoiseLite noise;
	int seed;
};