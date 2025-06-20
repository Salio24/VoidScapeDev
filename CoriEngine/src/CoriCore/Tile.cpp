#include "Tile.hpp"
#include "AssetManager/AssetManager.hpp"

namespace Cori {

	Tile::Tile(const glm::vec2 position, const glm::vec2 size, const SpriteAtlasDescriptor& descriptor, const uint32_t index) : m_Position(position), m_Size(size) {
		std::shared_ptr<SpriteAtlas> atlas = AssetManager::GetSpriteAtlas(descriptor); m_Sprite = Sprite::Create(atlas->GetTexture(), atlas->GetSpriteUVsAtIndex(index));
	}

	bool Tile::PreCreateHook(const glm::vec2 position, const glm::vec2 size, const SpriteAtlasDescriptor& descriptor, const uint32_t index) {
		return true;
	}
}
