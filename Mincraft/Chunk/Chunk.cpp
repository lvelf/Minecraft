#include "Chunk.h"


void Chunk::setBlock(int lx, int ly, int lz, BlockType type) {
	if (blocks[index(lx, ly, lz)].type != type)
		dirty = true;

	blocks[index(lx, ly, lz)].type = type;
}

bool Chunk::isAirLocal(int lx, int ly, int lz) const {
	if (lx < 0 || lx >= CHUNK_SIZE_X ||
		ly < 0 || ly >= CHUNK_SIZE_Y ||
		lz < 0 || lz >= CHUNK_SIZE_Z) {
		return true;
	}

	// TODO: ChunkManager should manage this, since it need be across chunks
	return blocks[index(lx, ly, lz)].type == BlockType::Air;
}

void Chunk::addFace(int wx, int wy, int wz, FaceDir dir) {
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

	unsigned int startIndex = static_cast<unsigned int>(vertices.size());

	for (int i = 0; i < 4; i++) {
		vertices.push_back(Vertex{ faceVerts[i] });
	}

	// 2 triangles 
	indices.push_back(startIndex + 0);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 0);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 3);

}

void Chunk::buildMesh() {

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

				// check whether around is air
				if (isAirLocal(lx + 1, ly, lz)) addFace(wx, wy, wz, FaceDir::PosX);
				if (isAirLocal(lx - 1, ly, lz)) addFace(wx, wy, wz, FaceDir::NegX);
				if (isAirLocal(lx, ly + 1, lz)) addFace(wx, wy, wz, FaceDir::PosY);
				if (isAirLocal(lx, ly - 1, lz)) addFace(wx, wy, wz, FaceDir::NegY);
				if (isAirLocal(lx, ly, lz + 1)) addFace(wx, wy, wz, FaceDir::PosZ);
				if (isAirLocal(lx, ly, lz - 1)) addFace(wx, wy, wz, FaceDir::NegZ);
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

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,               // layout(location = 0)
			3,               // vec3
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)0
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