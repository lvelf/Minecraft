#pragma once

#include <map>
#include <memory>
#include "Block.h"
#include "Chunk.h"

class ChunkManager {
public:

	std::map<ChunkPos, std::unique_ptr<Chunk>> chunks;

	Chunk* getChunk(const ChunkPos& pos);

	Chunk* getOrCreateChunk(const ChunkPos& pos, BlockType fillType = BlockType::Air);

	void worldToChunk(int bx, int by, int bz, ChunkPos& outChunkPos, int& lx, int& ly, int& lz) const;
	
	// interface for change block in the world
	Block getBlockWorld(int bx, int by, int bz);
	void setBlockWorld(int bx, int by, int bz, BlockType type);

	void renderAll();

private:
	int floorDiv(int a, int b) const;
	int floorMod(int a, int b) const;
};
