#pragma once
#include "../Chunk/ChunkManager.h"
#include "TerrainGenerator.h"


class WorldManager {
public:
	WorldManager(int radiusChunks, int worldHeight) : radius(radiusChunks), worldHeight(worldHeight) {
		generateInitialWolrd();
	}

	void generateInitialWolrd();

	void render();

	ChunkManager& getChunkManager() {
		return chunkManager;
	}

private:
	int renderRaius;
	
	ChunkManager chunkManager;
	TerrainGenerator terrainGenerator;
	int radius;
	int worldHeight;

	void generateChunk(int x, int y, int z);
};