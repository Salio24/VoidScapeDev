#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "GameObject.hpp"
#include <random>
#include <vector>
#include "../Physics/CollisionHandler.hpp"
#include "Actor.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/exterior_product.hpp>
#include "../Graphics/AnimationHandler.hpp"
#include <SDL3/SDL_mixer.h>
#include "../InternalLogic/StateMachine.hpp"

class BlackHole : public GameObject {
public:
	BlackHole();

	~BlackHole();

	void Update(std::vector<GameObject>* blocks, Actor& actor, const float& deltaTime, Animation& loopAnim, Animation& birthAnim, Mix_Chunk* bornSound, Mix_Chunk* consumedSound, Mix_Chunk* blackHoleIdle, const float globalSFXVolumeModifier, DeathCause actorDeathCause);

	void Reset();
	 
	glm::vec2 AABBSize { 0.0f, 1080.0f };
	glm::vec2 epicenterAABBPos{ 0.0f, 520.0f };
	glm::vec2 epicenterAABBSize{ 40.0f, 40.0f };
private: 
	bool isBorn{ false };
	bool loopTimerOneShot{ false };
	bool birthTimerOneShot{ false };
	int idleVolume{ 0 };

	glm::vec2 AABBPos { 0.0f, 0.0f };

	float AABBVelocityMultiplier { 0.0f };

	// squared distance of idle volume range
	float idleRadius{ 1250000.0f };

	std::vector<std::pair<int, float>> affectedBlocks;

	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<float> randomVelocity;
};