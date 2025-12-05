#include "Block.h"

std::string blockName(Block block) {

		if (block.type == BlockType::Air) {
			return "Air";
		}
		if (block.type == BlockType::Dirt) {
			return "Dirt";
		}

		if (block.type == BlockType::Grass) {
			return "Grass";
		}

		if (block.type == BlockType::Stone) {
			return "Stone";
		}

		if (block.type == BlockType::Water) {
			return "Water";
		}

		if (block.type == BlockType::Sand) {
			return "Sand";
		}

		if (block.type == BlockType::StoneBricks) {
			return "StoneBricks";
		}
}