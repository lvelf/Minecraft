#include "WorldManager.h"

void WorldManager::generateInitialWolrd() {
	for (int cx = -radius; cx <= radius; ++cx) {
		for (int cz = -radius; cz <= radius; ++cz) {
			int groundY = terrainGenerator.getHeight(cx, cz);
			
			for (int wy = -10; wy <= worldHeight; ++wy) {
				BlockType blockType = terrainGenerator.chooseBlock(cx, wy, cz, groundY);
				chunkManager.setBlockWorld(cx, wy, cz, blockType);
			}
			
		}
	}
}

void WorldManager::generateChunk(int x, int y, int z) {
	
}

void WorldManager::render() {
	chunkManager.renderAll();
}