#include "Chunk.h"


void Chunk::setBlock(int lx, int ly, int lz, BlockType type) {
	if (blocks[index(lx, ly, lz)].type != type)
		dirty = true;

	blocks[index(lx, ly, lz)].type = type;
}

void Chunk::buildMesh() {
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

	vertices.clear();
	indices.clear();
	
	// test
	float y = 0.0f;
	vertices.push_back({ 0.0f, y, 0.0f });
	vertices.push_back({ 1.0f, y, 0.0f });
	vertices.push_back({ 1.0f, y, 1.0f });
	vertices.push_back({ 0.0f, y, 1.0f });

	indices = {
		0,1,2,
		0,2,3
	};

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