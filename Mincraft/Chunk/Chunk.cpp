#include "Chunk.h"
#include <iostream>
#include "ChunkManager.h"

void Chunk::setBlock(int lx, int ly, int lz, BlockType type) {
	if (blocks[index(lx, ly, lz)].type != type)
		dirty = true;

	blocks[index(lx, ly, lz)].type = type;
}

bool Chunk::isAirLocal(int lx, int ly, int lz, ChunkManager& chunkManager) const {
	if (lx < 0 || lx >= CHUNK_SIZE_X ||
		ly < 0 || ly >= CHUNK_SIZE_Y ||
		lz < 0 || lz >= CHUNK_SIZE_Z) {
		
		int wx = pos.x * CHUNK_SIZE_X + lx;
		int wy = pos.y * CHUNK_SIZE_Y + ly;
		int wz = pos.z * CHUNK_SIZE_Z + lz;

		Block neighbourBlock = chunkManager.getBlockWorld(wx, wy, wz);

		return neighbourBlock.type == BlockType::Air;
	}

	// TODO: ChunkManager should manage this, since it need be across chunks
	return blocks[index(lx, ly, lz)].type == BlockType::Air;
}

TileIndex Chunk::getTileIndex(BlockType type, FaceDir dir) {
	switch (type) {
	case BlockType::Stone:
		return { 0, 0 };
	case BlockType::StoneBricks:
		return { 1, 0 }; // stone_bricks

	case BlockType::Water:
		return { 2, 0 }; // water_still frame

	case BlockType::Dirt:
		return { 2, 1 }; // dirt

	case BlockType::Sand:
		return { 3, 1 }; // sand

	case BlockType::Grass:
		if (dir == FaceDir::PosY)       // top grass_top
			return { 0, 1 };            
		if (dir == FaceDir::NegY)      // bottom dirt
			return { 2, 1 };             
		
		return { 1, 1 };                 // grass_side

	default:
		return { 0, 0 };                 // fallback: stone
	}
}

void Chunk::addFace(int wx, int wy, int wz, FaceDir dir, BlockType type) {
	glm::vec3 base(wx, wy, wz);

	std::array<glm::vec3, 4> faceVerts;

	switch (dir) {
	case FaceDir::PosX: // +X
		faceVerts = {
			base + glm::vec3(1, 0, 0),
			base + glm::vec3(1, 1, 0),
			base + glm::vec3(1, 1, 1),
			base + glm::vec3(1, 0, 1)
		};
		break;
	case FaceDir::NegX: // -X 
		faceVerts = {
			base + glm::vec3(0, 0, 1),
			base + glm::vec3(0, 1, 1),
			base + glm::vec3(0, 1, 0),
			base + glm::vec3(0, 0, 0)
		};
		break;
	case FaceDir::PosY: // +Y
		faceVerts = {
			base + glm::vec3(0, 1, 1),
			base + glm::vec3(1, 1, 1),
			base + glm::vec3(1, 1, 0),
			base + glm::vec3(0, 1, 0)
		};
		break;
	case FaceDir::NegY: // -Y 
		faceVerts = {
			base + glm::vec3(0, 0, 0),
			base + glm::vec3(1, 0, 0),
			base + glm::vec3(1, 0, 1),
			base + glm::vec3(0, 0, 1)
		};
		break;
	case FaceDir::PosZ: // +Z 
		faceVerts = {
			base + glm::vec3(0, 0, 1),
			base + glm::vec3(1, 0, 1),
			base + glm::vec3(1, 1, 1),
			base + glm::vec3(0, 1, 1)
		};
		break;
	case FaceDir::NegZ: // -Z 
		faceVerts = {
			base + glm::vec3(1, 0, 0),
			base + glm::vec3(0, 0, 0),
			base + glm::vec3(0, 1, 0),
			base + glm::vec3(1, 1, 0)
		};
		break;
	}

	// normal

	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	switch (dir) {
	case FaceDir::PosX:
		normal = glm::vec3(1, 0, 0);
		tangent = glm::vec3(0, 0, 1);
		bitangent = glm::vec3(0, 1, 0);
		break;
	case FaceDir::NegX:
		normal = glm::vec3(-1, 0, 0);
		tangent = glm::vec3(0, 0, -1);
		bitangent = glm::vec3(0, 1, 0);
		break;
	case FaceDir::PosY:
		normal = glm::vec3(0, 1, 0);
		tangent = glm::vec3(1, 0, 0);
		bitangent = glm::vec3(0, 0, -1);
		break;
	case FaceDir::NegY:
		normal = glm::vec3(0, -1, 0);
		tangent = glm::vec3(1, 0, 0);
		bitangent = glm::vec3(0, 0, 1);
		break;
	case FaceDir::PosZ:
		normal = glm::vec3(0, 0, 1);
		tangent = glm::vec3(1, 0, 0);
		bitangent = glm::vec3(0, 1, 0);
		break;
	case FaceDir::NegZ:
		normal = glm::vec3(0, 0, -1);
		tangent = glm::vec3(-1, 0, 0);
		bitangent = glm::vec3(0, 1, 0);
		break;
	}

	// uv
	TileIndex tile = getTileIndex(type, dir);
	const float cellX = 1.0f / 4.0f;
    float u0 = tile.cx * cellX;
    float u1 = (tile.cx + 1) * cellX;
	const float cellY = 1.0f / 2.0f;
    float v0 = tile.cy * cellY;
    float v1 = (tile.cy + 1) * cellY;
	
	const float eps = 0.001f;
	u0 += eps; v0 += eps;
	u1 -= eps; v1 -= eps;


	/*std::array<glm::vec2, 4> faceUV = {
		glm::vec2(u0, v1),
		glm::vec2(u1, v1),
		glm::vec2(u1, v0),
		glm::vec2(u0, v0),
	};*/
	std::array<glm::vec2, 4> faceUV;
	switch (dir) {
	case FaceDir::PosX: // +X 
		faceUV = {
			glm::vec2(u1, v1), 
			glm::vec2(u1, v0), 
			glm::vec2(u0, v0), 
			glm::vec2(u0, v1)  
		};
		break;

	case FaceDir::NegX: // -X 
		faceUV = {
			glm::vec2(u0, v1),
			glm::vec2(u0, v0),
			glm::vec2(u1, v0),
			glm::vec2(u1, v1)
		};
		break;

	case FaceDir::PosY: // +Y
		faceUV = {
			glm::vec2(u0, v0),
			glm::vec2(u1, v0),
			glm::vec2(u1, v1),
			glm::vec2(u0, v1)
		};
		break;

	case FaceDir::NegY: // -Y
		faceUV = {
			glm::vec2(u0, v1),
			glm::vec2(u1, v1),
			glm::vec2(u1, v0),
			glm::vec2(u0, v0)
		};
		break;

	case FaceDir::PosZ: // +Z
		faceUV = {
			glm::vec2(u0, v1),
			glm::vec2(u1, v1),
			glm::vec2(u1, v0),
			glm::vec2(u0, v0)
		};
		break;

	case FaceDir::NegZ: // -Z
		faceUV = {
			glm::vec2(u0, v1),
			glm::vec2(u1, v1),
			glm::vec2(u1, v0),
			glm::vec2(u0, v0)
		};
		break;
	}
	

	// material
	MaterialType mat = MaterialType::Default;
	switch (type) {
	case BlockType::Grass:
		mat = MaterialType::Grass;
		break;
	case BlockType::Dirt:
		mat = MaterialType::Dirt;
		break;
	case BlockType::Stone:
		mat = MaterialType::Stone;
		break;
	case BlockType::Sand:
		mat = MaterialType::Sand;
		break;
	case BlockType::Water:
		mat = MaterialType::Water;
		break;
	case BlockType::StoneBricks:
		mat = MaterialType::StoneBricks;
		break;
	default:
		mat = MaterialType::Default;
	}


	unsigned int startIndex = static_cast<unsigned int>(vertices.size());

	for (int i = 0; i < 4; i++) {
		vertices.push_back(Vertex{ faceVerts[i], normal, faceUV[i], tangent, bitangent, mat});
	}

	// 2 triangles 
	indices.push_back(startIndex + 0);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 0);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 3);

}

void Chunk::buildMesh(ChunkManager& chunkManager) {

	vertices.clear();
	indices.clear();

	for (int ly = 0; ly < CHUNK_SIZE_Y; ++ly) {
		for (int lz = 0; lz < CHUNK_SIZE_Z; ++lz) {
			for (int lx = 0; lx < CHUNK_SIZE_X; ++lx) {

				Block b = getBlock(lx, ly, lz);
				if (b.type == BlockType::Air) continue; // only draw block that is not air

				// world position
				int wx = pos.x * CHUNK_SIZE_X + lx;
				int wy = pos.y * CHUNK_SIZE_Y + ly;
				int wz = pos.z * CHUNK_SIZE_Z + lz;

				//static int debugCount = 0;
				

				// check whether around is air
				/*
				if (isAirLocal(lx + 1, ly, lz, chunkManager)) addFace(wx, wy, wz, FaceDir::PosX, b.type);
				if (isAirLocal(lx - 1, ly, lz, chunkManager)) addFace(wx, wy, wz, FaceDir::NegX, b.type);
				if (isAirLocal(lx, ly + 1, lz, chunkManager)) addFace(wx, wy, wz, FaceDir::PosY, b.type);
				if (isAirLocal(lx, ly - 1, lz, chunkManager)) addFace(wx, wy, wz, FaceDir::NegY, b.type);
				if (isAirLocal(lx, ly, lz + 1, chunkManager)) addFace(wx, wy, wz, FaceDir::PosZ, b.type);
				if (isAirLocal(lx, ly, lz - 1, chunkManager)) addFace(wx, wy, wz, FaceDir::NegZ, b.type);*/
				if (b.type != BlockType::Water) {
					addFace(wx, wy, wz, FaceDir::PosX, b.type);
					addFace(wx, wy, wz, FaceDir::NegX, b.type);
					addFace(wx, wy, wz, FaceDir::PosY, b.type);
					addFace(wx, wy, wz, FaceDir::NegY, b.type);
					addFace(wx, wy, wz, FaceDir::PosZ, b.type);
					addFace(wx, wy, wz, FaceDir::NegZ, b.type);
				}

				if (b.type == BlockType::Water) {
					if (isAirLocal(lx + 1, ly, lz, chunkManager)) addFace(wx, wy, wz, FaceDir::PosX, b.type);
					if (isAirLocal(lx - 1, ly, lz, chunkManager)) addFace(wx, wy, wz, FaceDir::NegX, b.type);
					if (isAirLocal(lx, ly + 1, lz, chunkManager)) addFace(wx, wy, wz, FaceDir::PosY, b.type);
					if (isAirLocal(lx, ly - 1, lz, chunkManager)) addFace(wx, wy, wz, FaceDir::NegY, b.type);
					if (isAirLocal(lx, ly, lz + 1, chunkManager)) addFace(wx, wy, wz, FaceDir::PosZ, b.type);
					if (isAirLocal(lx, ly, lz - 1, chunkManager)) addFace(wx, wy, wz, FaceDir::NegZ, b.type);
				}
				
			}
		}
	}


	// load vertices and indices to GPU(VBO/EBO)
	
	// generate VAO VBO EBO
	if (vao == 0) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		// layout = 0 position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,               // layout(location = 0)
			3,               // vec3
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, position)
		);

		// layout = 1 normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, normal)
		);

		// layout = 2 uv
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, uv)
		);

		// layout = 3 tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, tangent)
		);
		// layout = 4 bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(
			4,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, bitangent)
		);
		//layout = 6 material
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(
			5,
			1,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, material)
		);

		glBindVertexArray(0);
	}

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int),
		indices.data(),
		GL_STATIC_DRAW);

	glBindVertexArray(0);

	dirty = false;
}

void Chunk::render() {
	if (vao == 0 || indices.empty())
		return;

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,
		static_cast<GLsizei>(indices.size()),
		GL_UNSIGNED_INT,
		0);
	glBindVertexArray(0);
}