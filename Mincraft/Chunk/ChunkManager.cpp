#include "ChunkManager.h"

Chunk* ChunkManager::getChunk(const ChunkPos& pos) {
	auto it = chunks.find(pos);

	if (it == chunks.end()) return nullptr;
	
	return it->second.get();
}

Chunk* ChunkManager::getOrCreateChunk(const ChunkPos& pos, BlockType fillType) {
    // exit then return
    if (Chunk* existing = getChunk(pos)) {
        return existing;
    }

    // create
    auto chunkPtr = std::make_unique<Chunk>(pos, fillType);
    Chunk* rawPtr = chunkPtr.get();
    chunks.emplace(pos, std::move(chunkPtr));
    return rawPtr;
}

int ChunkManager::floorDiv(int a, int b) const {
    int q = a / b;
    int r = a % b;
    if ((r != 0) && ((r > 0) != (b > 0))) {
        --q;
    }
    return q;
}

int ChunkManager::floorMod(int a, int b) const {
    int r = a % b;
    if ((r != 0) && ((r > 0) != (b > 0))) {
        r += b;
    }
    return r;
}

void ChunkManager::worldToChunk(int bx, int by, int bz,
    ChunkPos& outChunkPos,
    int& lx, int& ly, int& lz) const
{
   
    // chun position
    outChunkPos.x = floorDiv(bx, CHUNK_SIZE_X);
    outChunkPos.y = floorDiv(by, CHUNK_SIZE_Y);
    outChunkPos.z = floorDiv(bz, CHUNK_SIZE_Z);
    
    // position in chunk
    lx = floorMod(bx, CHUNK_SIZE_X);
    ly = floorMod(by, CHUNK_SIZE_Y);
    lz = floorMod(bz, CHUNK_SIZE_Z);
}

Block ChunkManager::getBlockWorld(int bx, int by, int bz) {
    ChunkPos cpos;
    int lx, ly, lz;
    worldToChunk(bx, by, bz, cpos, lx, ly, lz);

    Chunk* chunk = getChunk(cpos);
    if (!chunk) {
        // if no this chunk it should be air
        return Block(BlockType::Air);
    }
    return chunk->getBlock(lx, ly, lz);
}

void ChunkManager::setBlockWorld(int bx, int by, int bz, BlockType type) {
    ChunkPos cpos;
    int lx, ly, lz;
    worldToChunk(bx, by, bz, cpos, lx, ly, lz);

    Chunk* chunk = getOrCreateChunk(cpos);
    chunk->setBlock(lx, ly, lz, type);
}

void ChunkManager::renderAll() {
    
    for (auto& kv : chunks) {
        const ChunkPos& pos = kv.first;
        Chunk* chunk = kv.second.get();

        if (chunk->dirty) {
            chunk->buildMesh(*this);
        }

        chunk->render();
    }

}