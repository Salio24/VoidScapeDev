#include "AssetManager.hpp"

namespace Cori {

	AssetManager& AssetManager::Get() {
		static AssetManager instance;
		return instance;
	}

	void AssetManager::PreloadShadersImpl(std::initializer_list<ShaderProgramDescriptor> descriptors) {
		CORI_CORE_INFO("AssetManager: Preloading {0} shader(s)", descriptors.size());
		for (const auto& descriptor : descriptors) {
			GetShaderImpl(descriptor);
		}
		CORI_CORE_INFO("AssetManager: Preloaded {0} shader(s)", descriptors.size());
	}

	void AssetManager::PreloadTexture2DsImpl(std::initializer_list<Texture2DDescriptor> descriptors) {
		CORI_CORE_INFO("AssetManager: Preloading {0} Texture2D(s)", descriptors.size());
		for (const auto& descriptor : descriptors) {
			GetTexture2DImpl(descriptor);
		}
		CORI_CORE_INFO("AssetManager: Preloaded {0} Texture2D(s)", descriptors.size());
	}

	void AssetManager::PreloadSpriteAtlasesImpl(std::initializer_list<SpriteAtlasDescriptor> descriptors) {
		CORI_CORE_INFO("AssetManager: Preloading {0} SpriteAtlas(es)", descriptors.size());
		for (const auto& descriptor : descriptors) {
			GetSpriteAtlasImpl(descriptor);
		}
		CORI_CORE_INFO("AssetManager: Preloaded {0} SpriteAtlas(es)", descriptors.size());
	}

	std::shared_ptr<ShaderProgram> AssetManager::GetShaderImpl(const ShaderProgramDescriptor& descriptor) {
		CORI_PROFILE_FUNCTION();
		auto it = m_ShaderCache.find(descriptor.GetRuntimeID());
		if (it != m_ShaderCache.end()) {
			return it->second;
		}

		CORI_CORE_DEBUG("AssetManager: Shader cache miss for {0} (RuntimeID: {1}). Loading...", descriptor.GetDebugName(), descriptor.GetRuntimeID());
		try {
			std::shared_ptr<ShaderProgram> newShader = ShaderProgram::Create(descriptor);
			m_ShaderCache[descriptor.GetRuntimeID()] = newShader;
			return newShader;
		}
		catch (const std::exception& e) {
			CORI_CORE_ERROR("AssetManager: Error loading shaders {0}: {1}", descriptor.GetDebugName(), e.what());
			return nullptr;
		}
	}

	std::shared_ptr<Texture2D> AssetManager::GetTexture2DImpl(const Texture2DDescriptor& descriptor) {
		CORI_PROFILE_FUNCTION();
		auto it = m_Texture2DCache.find(descriptor.GetRuntimeID());
		if (it != m_Texture2DCache.end()) {
			return it->second;
		}

		CORI_CORE_DEBUG("AssetManager: Texture2D cache miss for {0} (RuntimeID: {1}). Loading...", descriptor.GetDebugName(), descriptor.GetRuntimeID());
		try {
			std::shared_ptr<Texture2D> newTexture2D = Texture2D::Create(descriptor);
			m_Texture2DCache[descriptor.GetRuntimeID()] = newTexture2D;
			return newTexture2D;
		}
		catch (const std::exception& e) {
			CORI_CORE_ERROR("AssetManager: Error loading Texture2D {0}: {1}", descriptor.GetDebugName(), e.what());
			return nullptr;
		}
	}

	std::shared_ptr<SpriteAtlas> AssetManager::GetSpriteAtlasImpl(const SpriteAtlasDescriptor& descriptor) {
		CORI_PROFILE_FUNCTION();
		auto it = m_SpriteAtlasCache.find(descriptor.GetRuntimeID());
		if (it != m_SpriteAtlasCache.end()) {
			return it->second;
		}

		CORI_CORE_DEBUG("AssetManager: SpriteAtlas cache miss for {0} (RuntimeID: {1}). Loading...", descriptor.GetDebugName(), descriptor.GetRuntimeID());
		try {
			std::shared_ptr<SpriteAtlas> newSpriteAtlas = SpriteAtlas::Create(descriptor);
			m_SpriteAtlasCache[descriptor.GetRuntimeID()] = newSpriteAtlas;
			return newSpriteAtlas;
		}
		catch (const std::exception& e) {
			CORI_CORE_ERROR("AssetManager: Error loading SpriteAtlas {0}: {1}", descriptor.GetDebugName(), e.what());
			return nullptr;
		}
	}
}