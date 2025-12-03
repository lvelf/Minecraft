#pragma once
#include "Block.h"
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <cstddef>

const int CHUNK_SIZE_X = 16;
const int CHUNK_SIZE_Y = 16;
const int CHUNK_SIZE_Z = 16;

struct ChunkPos {
	int x, y, z;

	bool operator<(const ChunkPos& other) const {
		if (x != other.x)
			return x < other.x;
		if (y != other.y)
			return y < other.y;
		return z < other.z;
	}
};

enum class FaceDir {
	PosX, NegX,
	PosY, NegY,
	PosZ, NegZ

};

struct Vertex {
	//float x, y, z;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;

	//Vertex(const glm::vec3& pos) : x(pos.x), y(pos.y), z(pos.z) {}

	//Vertex(float x, float y, float z) : x(x), y(y), z(z) {}

	Vertex(const glm::vec3 pos, const glm::vec3 n, const glm::vec2 uv) :
		position(pos), normal(n), uv(uv) {}
};

// Tile uv in atlas
struct TileIndex {
	int cx;
	int cy;
};

class Chunk {
public:
	ChunkPos pos;
	std::vector<Block> blocks;

	// OpenGL
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ebo = 0;


	bool dirty = true; // regenerate mesh

	Chunk(ChunkPos p) : pos(p) {
		blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, Block(BlockType::Air));
	}

	Chunk(ChunkPos p, BlockType type) : pos(p) {
		blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, Block(type));
	}

	inline int index(int lx, int ly, int lz) const {
		// x + z * X + y * X * Z
		return lx + lz * CHUNK_SIZE_X + ly * CHUNK_SIZE_X * CHUNK_SIZE_Z;
	}

	Block getBlock(int lx, int ly, int lz) const {
		return blocks[index(lx, ly, lz)];
	}

	void setBlock(int lx, int ly, int lz, BlockType type);


	// vertices and indice
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	//mesh
	void buildMesh(); // generate vertices/indices and upload to GPU
	TileIndex getTileIndex(BlockType type, FaceDir dir); // generate uv
	void addFace(int wx, int wy, int wz, FaceDir dir, BlockType type); // world block position + dir

	bool isAirLocal(int lx, int ly, int lz) const; // whether need to be drawn

	void render(); // bind VAO + Draw
};