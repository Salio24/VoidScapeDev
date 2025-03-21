#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "GameObject.hpp"
#include "../Graphics/AnimationHandler.hpp"
#include "glm/glm.hpp"
#include "Actor.hpp"
#include "../Physics/CollisionHandler.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/exterior_product.hpp>
#include <SDL3/SDL_mixer.h>

class EscapePortal : public GameObject {
public:
	EscapePortal();

	~EscapePortal();

	void Update(Animation& portalAnim, const float& deltaTime, Actor& actor, Mix_Chunk* portalSoundEscape, Mix_Chunk* portalSoundIdle, const float globalSFXVolumeModifier);

	float mSizeMultiplier{ 200.0f };

private:
	bool animationOneShot{ true };

	glm::vec2 AABBPos{ 0.0f, 0.0f };
	glm::vec2 AABBSize{ 0.0f, 0.0f };
	float suctionVelocity{ 0.0f };

	glm::vec2 epicenterAABBPos{ 0.0f, 0.0f };
	glm::vec2 epicenterAABBSize{ 0.0f, 0.0f };

	int idleVolume{ 0 };
	// squared distance of idle volume range
	float idleRadius{ 1250000.0f };

};