#pragma once
#include "../Chunk/Chunk.h"
#include "../Chunk/Block.h"
#include "FastNoiseLite.h"

struct TerrainParams {
	float baseHeight = 5.0f;
	float amplitude = 8.0f;
	float frequency = 0.02f;

	float biomeFrequency = 0.005f;
	float caveFrequency = 0.05f;
};

enum class BiomeType {
	Ocean, 
	Desert,
	Plains
};

class TerrainGenerator {
public:
	TerrainGenerator(const TerrainParams& params = TerrainParams(), int seed1 = 1337, int seed2 = 1338, int seed3 = 1339) : params(params) {

		heightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		heightNoise.SetSeed(seed1);
		heightNoise.SetFrequency(params.frequency);

		biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		biomeNoise.SetSeed(seed2);
		biomeNoise.SetFrequency(params.biomeFrequency);
		
		caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		caveNoise.SetSeed(seed3);
		caveNoise.SetFrequency(params.caveFrequency);
	};

	// wx wz -> wy
	int getHeight(int wx, int wz) const;

	BlockType chooseBlock(int wx, int wy, int wz, int groundY) const;



private:
	TerrainParams params;
	FastNoiseLite heightNoise;
	FastNoiseLite biomeNoise;
	FastNoiseLite caveNoise;
	

	BiomeType getBiome(int wx, int wz, int groundY) const;
	bool isCave(int wx, int wy, int wz) const;
	
};