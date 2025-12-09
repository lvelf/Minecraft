#include "TerrainGenerator.h"
#include <iostream>
int TerrainGenerator::getHeight(int wx, int wz) const {
	
	float h = heightNoise.GetNoise((float)wx, (float)wz);
	h = (h * 0.5f + 0.5f);

	int height = (int)(params.baseHeight + h * params.amplitude);
	
	return height;
}

BiomeType TerrainGenerator::getBiome(int wx, int wz, int groundY) const {
	const int waterLevel = 8;

	

	float b = biomeNoise.GetNoise(float(wx), float(wz));// [-1, 1]
	float b01 = b * 0.5f + 0.5f; // [0, 1]

	if (b01 < 0.33f)
		return BiomeType::Ocean;
	if (b01 < 0.66f)
		return BiomeType::Desert;
	return BiomeType::Plains;
}

BlockType TerrainGenerator::getLand(int wx, int wy, int wz) const {
	const int waterLevel = 8;

	float b = landNoise.GetNoise(float(wx), float(wy), float(wz));// [-1, 1]
	float b01 = b * 0.5f + 0.5f; // [0, 1]

	if (b01 < 0.25f)
		return BlockType::Stone;
	if (b01 < 0.50f)
		return BlockType::Sand;
	if (b01 < 0.75f)
		return BlockType::StoneBricks;
	return BlockType::Dirt;
}

bool TerrainGenerator::isCave(int wx, int wy, int wz) const {
	if (wy >= -4)
		return false;
	float n = caveNoise.GetNoise((float)wx, (float)wy * 0.7f, (float)wz); //[-1, 1]

	const float threshold = 0.6f;
	return n > threshold;
}

int TerrainGenerator::getDepth(int wx, int wz) const {
	float n = depthNoise.GetNoise((float)wx, (float)wz); //[-1, 1]
	float d01 = n * 0.5f + 0.5f; // [0, 1]
	d01 = d01 * d01;
	float depth = params.minDepth + d01 * (params.maxDepth - params.minDepth);
	return depth;
}




BlockType TerrainGenerator::chooseBlock(int wx, int wy, int wz, int groundY) const {
	
	int depth = getDepth(wx, wz);

	BiomeType biome = getBiome(wx, wz, groundY);
	int oceanLevel = 12;

    switch (biome) {
    case BiomeType::Ocean:
		if (wy > groundY)
			return BlockType::Air;
		else if (wy <= groundY && wy >= groundY - depth && wy <= oceanLevel)
			return BlockType::Water;
		else
			return getLand(wx, wy, wz);
        

    case BiomeType::Desert:
		if (wy > groundY)
			return BlockType::Air;
		else if (wy <= groundY && wy >= groundY - depth)
			return BlockType::Sand;
		else
			return getLand(wx, wy, wz);
        

    case BiomeType::Plains:
    default:
		if (wy > groundY)
			return BlockType::Air;
		else if (wy <= groundY && wy >= groundY - depth)
			return BlockType::Grass;
		else
			return getLand(wx, wy, wz);
        
    }
	

	return BlockType::Stone;
}