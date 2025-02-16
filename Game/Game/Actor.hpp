#pragma once
#include "GameEntity.hpp"
#include <glm/vec2.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "CollisionHandler.hpp"

enum class MovementDirection {
	None,
	Left,
	Right,
	Top,
	Bottom
};


class Actor : public GameEntity {
public:
	Actor();

	~Actor();

	void Update() override;

	void Move();

	void Transform();

	void Reset(glm::vec2& animSize, const glm::vec2& startingPosition);

	MovementDirection mDirection{ MovementDirection::None };

	glm::vec2 mScreenPosition{ 0.0f, 0.0f };

	float mFlyAngleTargetPortal{ -1.0f };
	float mFlyAnglePortal{ 0.0f };

	bool mIsWallMountableL{ false };
	bool mIsWallMountableR{ false };

	bool mEscaped{ false };

	bool mDead{ false };

	bool mDeadSoundOneShot{ true };

	bool mFellDown{ false };

	bool mDiedFromTrigger{ false };

	bool mIsSuckedPortal{ false };

	float mSizeMultiplier{ 60.0f };

	Box mDoubleJumpOrb;

private:
	glm::vec2 relativePosition{ 0.0f, 0.0f };
};