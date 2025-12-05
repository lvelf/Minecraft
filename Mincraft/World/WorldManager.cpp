#include "WorldManager.h"
#include <iostream>
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

void WorldManager::createExampleWorld() {

		int minX = -16;
		int maxX = 16;
		int minZ = -16;
		int maxZ = 16;

		std::cout << "create example world" << std::endl;

		for (int x = minX; x < maxX; ++x) {
			for (int z = minZ; z < maxZ; ++z) {
				int groundY = 0;

				// y < -5: Stone
				for (int y = -8; y < -5; ++y) {
					chunkManager.setBlockWorld(x, y, z, BlockType::Sand);
				}

				// y < -3: Stone
				for (int y = -5; y <= -3; ++y) {
					chunkManager.setBlockWorld(x, y, z, BlockType::Stone);
				}

				// -3 <= y < -1: Water
				for (int y = -3; y < -1; ++y) {
					chunkManager.setBlockWorld(x, -1, z, BlockType::Water);
				}

				
				

			}
		}


		for (int x = -5; x <= 5; ++x) {
			for (int z = -5; z <= 5; ++z) {
				int height = 1 + (std::abs(x) + std::abs(z)) % 4;
				for (int h = 0; h < height; ++h) {
					chunkManager.setBlockWorld(x, -1 + h, z, BlockType::Dirt);
				}

				chunkManager.setBlockWorld(x, -1 + height, z, BlockType::Grass);
			}
		}
	
}