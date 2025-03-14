#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <chrono>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "../Utilities/Sign.hpp"
#include "../Utilities/HelperStructs.hpp"
#include "../Actors/GameObject.hpp"


struct PhysicsSettings {
	float GravityUP{ -900.0f };
	float GravityDOWN{ -1200.0f };
	float FallSpeedLimit{ -1500.0f };

	float RunAccelerationOnFoot{ 1000.0f };
	float RunAccelerationMidAir{ 400.0f };
	float RunSpeedLimit{ 800.0f };

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
	int VariableJumpTicks{ 32 };
	int JumpBufferTime{ 200 };

	float Drag{ 0.0f };
	float Bounce{ 0.0f };
};

struct PhysicsState
{
	glm::vec2 position{ 0.0f };
	glm::vec2 velocity{ 0.0f };
};

struct Derivative
{
	glm::vec2 PositionDerivative{ 0.0f };
	glm::vec2 VelocityDerivative{ 0.0f };
};

class PhysicsComponent {
public:
	PhysicsComponent();

	~PhysicsComponent();

	glm::vec2 MultiplyVec2AndDouble(glm::vec2 vec, double d) const;

	Derivative RK4_EvaluateVec2(PhysicsState& initial, double time, double timeStep, const Derivative& d);

	glm::vec2 RK4_AccelerationVec2(const PhysicsState& state, double time);

	void RK4_IntegrateVec2(PhysicsState& state, double time, double timeStep);

	void FixedTickrateUpdate(double deltaTime, const std::vector<GameObject>* blocks, bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], glm::vec2 colliderSize);

	std::pair<glm::vec2, glm::vec2> Update(double accumulator, double timeStep);

	void SetPosition(glm::vec2 position);

	bool PointVsRect(const glm::vec2& point, const glm::vec2& boxSize, const glm::vec2& boxPos);

	bool RectVsRect(const glm::vec2 rect1Pos, const glm::vec2 rect1Size, const glm::vec2 rect2Pos, const glm::vec2 rect2Size);

	bool RayVsRect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const Box* target, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear);

	void CollisionUpdate(const std::vector<GameObject>* blocks, glm::vec2 previousPos, glm::vec2& currentPos, glm::vec2& velocity, double timeStep, glm::vec2 colliderSize);

	void ResolvePenetration(const std::vector<GameObject>* blocks, glm::vec2& position, glm::vec2& velocity, glm::vec2 size);

	void MovementUpdate(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)]);

	glm::vec2 testV{ 0.0f };
	PhysicsState mInterpolatedState;

	glm::vec2 pos = glm::vec2(0.0f, 800.0f);

	LookDirections mLookDirection = LookDirections::RIGHT;

	bool testButton1{ false };
	bool testButton2{ false };
	bool testButton3{ false };
	bool testButton4{ false };

	bool mGrounded{ false };

	bool mSliding{ false };

	bool mJumping{ false };

	// active movement on ground
	bool mActiveRunning{ false };

	// passive movement on ground (no input)
	bool mPassiveRunning{ false };


	

	glm::vec2 accF{ 0.0f };

	glm::vec2 testPos{ 0.0f };	
	
	std::vector<glm::vec2> collBlocks;

	PhysicsSettings mPhysicsSettings;

	bool mSpacebarOneShot{ true };

	bool mDuckOneShot{ true };


private:
	double time = 0.0f;
	double accumulator = 0.0f;
	double timeStep = 1.0f / 128.0f;

	glm::vec2 acceleration{ 0.0f };

	PhysicsState previous;
	PhysicsState current;

	glm::vec2 normal;
	glm::vec2 fric;

	int jumpTickTimer{ 0 };

	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> jumpBufferTimer;

	glm::vec2 colAcc{ 0.0f };

};

