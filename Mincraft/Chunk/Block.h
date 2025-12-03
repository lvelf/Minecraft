#pragma once
#include <cstdint>

enum class BlockType : uint8_t {
	Air = 0,
	Grass,
	Dirt,
	Stone, 
	Water,
	Sand,
	StoneBricks
};

enum class MaterialType : int {
	Default = 0,
	Grass = 1,
	Dirt,
	Stone, 
	Water, 
	Sand,
	StoneBricks
};

struct Block {
	BlockType type;

	Block(BlockType type) : type(type) {}
};

