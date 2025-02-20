#pragma once
#include <glm/glm.hpp>
#include <chrono>
#include "Actor.hpp"
#include "HelperStructs.hpp"

enum class MovementState {
	MOVE_LEFT,
	MOVE_RIGHT,
	SPACE,
	DUCK,
	MOVE_UP,
	MOVE_DOWN,
	END
};

enum class LookDirections {
	LEFT,
	RIGHT,
	END
};

class MovementHandler {
public:
	MovementHandler();

	~MovementHandler();

	void FinishDoubleJump();

	void FinishJump();

	bool mCanDoubleJump{ false };
	bool mLast_mCanDoubleJump{ false };
	bool mIsGrounded{ false };
	bool mIsSlamming{ false };

	bool mIsSliding{ false };
	bool mCanWallStick{ false };

	bool mIsWallMountableL{ false };
	bool mIsWallMountableR{ false };

	void Update(float& deltaTime, Actor& actor, const bool& started, Config& config);

	bool mSpacebarOneShot{ true };

	bool mDuckOneShot{ true };

	bool mKeyboadStates[static_cast<int>(MovementState::END)] = { false };

	LookDirections mLookDirection = LookDirections::RIGHT;

	bool mLeftWallHug{ false };
	bool mRightWallHug{ false };

private:
	glm::vec2 acceleration{ 0.0f, 0.0f };

	void Move(glm::vec2& actorVelocity, const glm::vec2& acceleration);

	void Jump(float& deltaTime, const float& jumpSpeed, glm::vec2& actorVelocity);

	void Slam(float& deltaTime, const float& slamSpeed, const float& speedLimit, glm::vec2& actorVelocity);

	float frictionModifier{ 7.2f };

	//const int jumpBufferTime{ 100 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> jumpBufferTimer;

	//const int jumpTime{ 200 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> jumpTimer;

	//const int wallJumpTime{ 200 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> wallJumpTimer;

	//const int doubleJumpTime{ 150 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> doubleJumpTimer;

	bool isJumping{ false };

	bool isWallJumping{ false }; 

	bool isDoubleJumping{ false };

	bool slideOneShot{ true };

	int slideDirection{ 0 };
};


