#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <chrono>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "../Utilities/Sign.hpp"
#include "../Utilities/HelperStructs.hpp"
#include "../Actors/GameObject.hpp"
#include "VSMath.hpp"

/*
struct PhysicsSettings {
	float GravityUP{ -900.0f };
	float GravityDOWN{ -1200.0f };
	float FallSpeedLimit{ -1500.0f };

	float RunAccelerationOnFoot{ 1000.0f };
	float RunAccelerationMidAir{ 400.0f };
	float RunSpeedLimit{ 800.0f };

	float CrouchAcceleration{ 1000.0f };
	float CrouchSpeedLimit{ 200.0f };

	float DefaultFrictionStopSpeed{ 20.0f };
	float DefaultFriction{ 2000.0f };
	// values > ~3800.0f are unstable 

	float AirResistanceStopSpeed{ 10.0f };
	float DefaultAirResistance{ 300.0f };
	// values > ~3800.0f are unstable 

	float SlidingFrictionStopSpeed{ 20.0f };
	float SlidingFriction{ 1000.0f };
	// values > ~3800.0f are unstable 

	float SlidingImpulse{ 50000.0f };
	float ThresholdSpeedToSlide{ 300.0f };

	float JumpStartImpulse{ 50000.0f };
	float JumpContinuousAcceleration{ 2000.0f };

	float DoubleJumpStartImpulse{ 50000.0f };
	float DoubleJumpContinuousAcceleration{ 2000.0f };

	float WallJumpStartImpulse{ 50000.0f };
	float WallJumpStartSideImpulse{ 25000.0f };
	float WallJumpContinuousAcceleration{ 2000.0f };

	int VariableJumpTicks{ 32 };
	int VariableDoubleJumpTicks{ 32 };
	int VariableWallJumpTicks{ 32 };

	int JumpBufferTicks{ 25 };

	int CoyoteTimeTicks{ 12 };

	float WallSlideSpeed{ 125.0f };

	float SlidingCollidorFactor{ 0.75f };
};
*/

class PhysicsComponent {
public:
	PhysicsComponent();

	~PhysicsComponent();

	void FixedTickrateUpdate(double timeStep, const std::vector<GameObject>* blocks, bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], glm::vec2 colliderSize);

	std::pair<glm::vec2, glm::vec2> Update(double accumulator, double timeStep);

	void SetPosition(glm::vec2 position);

	void CollisionUpdate(const std::vector<GameObject>* blocks, glm::vec2 colliderSize, bool activeKeys[static_cast<int>(ActiveKeys::DUCK)]);

	void MovementUpdate(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], double timeStep);

	bool GetDeath();

	LookDirections mLookDirection = LookDirections::RIGHT;

	bool mWallHugLeft{ false };

	bool mWallHugRight{ false };

	bool mGrounded{ false };

	bool mCoyoteTimeActive{ false };

	bool mSliding{ false };

	bool mJumping{ false };

	// active movement on ground
	bool mActiveRunning{ false };

	// passive movement on ground (no input)
	bool mPassiveRunning{ false };

	bool mDoubleJumpAvailable{ false };

	bool mDoubleJumping{ false };

	bool mWallJumping{ false };

	bool mFastWallSlide{ false };

	bool mCanStand{ true };

	bool mCrouching{ false };

	Config mPhysicsSettings;

	bool mSpacebarOneShot{ true };

	bool mDuckOneShot{ true };

	bool testButton4{ false };
	bool testButton3{ false };


private:
	void HandleHorizontalMovement(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], double timeStep);
	void HandleGravityAndFalling(double timeStep);
	void HandleStateMachine(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)]);
	void HandleFrictionAndResistance(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], double timeStep);
	void HandleSlide(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)]);
	void HandleJumpsAndCoyoteTime(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], double timeStep);
	
	glm::vec2 acceleration{ 0.0f };

	PhysicsState previous;
	PhysicsState current;

	glm::vec2 normal;

	int jumpTickTimer{ -1 };

	int doubleJumpTickTimer{ -1 };

	int wallJumpTickTimer{ -1 };

	int jumpBufferTimer{ -1 };

	int coyoteTimeTimer{ -1 };

	bool death1{ false };

	bool m_NearLeftWall{ false };
	bool m_NearRightWall{ false };

	bool m_WasNearLeftWall{ false };
	bool m_WasNearRightWall{ false };
};

