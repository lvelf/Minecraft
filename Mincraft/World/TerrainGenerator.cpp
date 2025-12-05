#include "TerrainGenerator.h"

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

bool TerrainGenerator::isCave(int wx, int wy, int wz) const {
	if (wy >= -4)
		return false;
	float n = caveNoise.GetNoise((float)wx, (float)wy * 0.7f, (float)wz); //[-1, 1]

	const float threshold = 0.6f;
	return n > threshold;
}



BlockType TerrainGenerator::chooseBlock(int wx, int wy, int wz, int groundY) const {
	const int waterLevel = 8;
	const int grassDepth = 3;
	const int sandDepth = 4; 
	const int caveMinDepth = 5; 

	BiomeType biome = getBiome(wx, wz, groundY);

    if (wy > groundY) {
        if (wy <= waterLevel) {
            return BlockType::Water;
        }
        return BlockType::Air;
    }

    
    if (groundY - wy >= caveMinDepth) {
        if (isCave(wx, wy, wz)) {
            return BlockType::Air;
        }
    }

    
    int depthBelowSurface = groundY - wy;

    switch (biome) {
    case BiomeType::Ocean:
       
        if (depthBelowSurface <= sandDepth) {
            return BlockType::Sand;
        }
        else {
            return BlockType::Stone;
        }

    case BiomeType::Desert:
        
        if (depthBelowSurface <= sandDepth + 2) {
            return BlockType::Sand;
        }
        else {
            return BlockType::Stone;
        }

    case BiomeType::Plains:
    default:
        if (depthBelowSurface <= grassDepth) {
            return BlockType::Dirt;
        }
        else {
            return BlockType::Stone;
        }
    }
	

	return BlockType::Stone;
}