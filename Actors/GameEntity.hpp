#pragma once
#include <glm/vec2.hpp>
#include "GameObject.hpp"

class GameEntity : public GameObject {
public:
	GameEntity();

	~GameEntity();

	virtual void Update();

	virtual void Reset();

	glm::vec2 mVelocity{ 0.0f, 0.0f };
};