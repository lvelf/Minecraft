#pragma once
#include "../Chunk/Chunk.h"
#include "../Chunk/Block.h"
#include "FastNoiseLite.h"

struct TerrainParams {
	float baseHeight = 5.0f;
	float amplitude = 16.0f;
	float frequency = 0.05f;

	float biomeFrequency = 0.005f;
	float caveFrequency = 0.05f;
	float oceanFrequency = 0.01f;
	float landFrequency = 0.05f;
	float minDepth = 5.0f;
	float maxDepth = 25.0f;
};

enum class BiomeType {
	Ocean, 
	Desert,
	Plains
};

class TerrainGenerator {
public:
	TerrainGenerator(const TerrainParams& params = TerrainParams(), int seed1 = 1337, int seed2 = 1338, int seed3 = 1339, int seed4 = 1440, int seed5 = 1441) : params(params) {

		heightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		heightNoise.SetSeed(seed1);
		heightNoise.SetFrequency(params.frequency);

		biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		biomeNoise.SetSeed(seed2);
		biomeNoise.SetFrequency(params.biomeFrequency);
		
		caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		caveNoise.SetSeed(seed3);
		caveNoise.SetFrequency(params.caveFrequency);

		depthNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		depthNoise.SetSeed(seed4);
		depthNoise.SetFrequency(params.oceanFrequency);

		landNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		landNoise.SetSeed(seed5);
		landNoise.SetFrequency(params.oceanFrequency);
	};

	// wx wz -> wy
	int getHeight(int wx, int wz) const;

	BlockType chooseBlock(int wx, int wy, int wz, int groundY) const;



private:
	TerrainParams params;
	FastNoiseLite heightNoise;
	FastNoiseLite biomeNoise;
	FastNoiseLite caveNoise;
	FastNoiseLite depthNoise;
	FastNoiseLite landNoise;
	

	BiomeType getBiome(int wx, int wz, int groundY) const;
	BlockType getLand(int wx, int wy, int wz) const;
	bool isCave(int wx, int wy, int wz) const;
	int getDepth(int wx, int wz) const;
};