#pragma once
#include "Texture.hpp"
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <vector>
#include <chrono>

struct Animation {
	glm::vec2 Size;
	glm::vec2 TexturePosition;
	std::vector<uint32_t> AnimationTextureIndexes;
	int AnimationTime;
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> AnimationTimer;
	int SingleFrameTime;
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> SingleFrameTimer;
	int index{ 0 };
};


class AnimationHandler {
public:
	AnimationHandler(TextureHandler& textureHandlerRef);
	~AnimationHandler();

	void Init();

	void InitMultiFrameAnimation(SDL_Surface* animationSpritesheet, Animation& animation, const glm::vec2& animationSize, const int& animationTime, const int& spriteResolution, const glm::vec2& texturePosition = glm::vec2(0.0f, 0.0f));

	void InitSingleFrameAnimation(SDL_Surface* animationSprite, Animation& animation, const glm::vec2& animationSize, const int& spriteResolution, const glm::vec2& texturePosition = glm::vec2(0.0f, 0.0f));

	Animation DeadAnimation;
	Animation DuckAnimation;
	Animation HitAnimation;
	Animation RunAnimation;
	Animation FallAnimation;
	Animation IdleAnimation;
	Animation JumpAnimation;
	Animation SlamAnimation;
	Animation SlideAnimation;
	Animation WallSlideAnimation;
	Animation DuckIdleAnimation;
	Animation BlackHoleBirthAnimation;
	Animation BlackHoleLoopAnimation;
	Animation EscapePortalAnimation;

private:
	TextureHandler&  textureHandlerRef;

	const int animationSpriteResolution{ 128 };
};