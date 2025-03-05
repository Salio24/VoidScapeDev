#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <chrono>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "../Actors//Actor.hpp"

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

	void FixedTickrateUpdate(double deltaTime, const std::vector<GameObject>* blocks);

	bool RayVsRectT(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const Box* target, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear);

	bool DynamicRectVsRectT(const glm::vec2& size, const float deltaTime, const Box& staticBox, const glm::vec2& dynamicBoxVelocity, glm::vec2& contactPoint, glm::vec2& contactNormal, float& contactTime, glm::vec2& position);

	bool ResolveDynamicRectVsRectT(const glm::vec2& size, const float deltaTime, const Box& staticBox, glm::vec2 dynamicBoxVelocity, glm::vec2& pos, glm::vec2& impulse);

	//void CUpdate(const std::vector<GameObject>* blocks, glm::vec2 pos, glm::vec2 size, glm::vec2 vel);

	void CUpdate(const std::vector<GameObject>* blocks, glm::vec2 previousPos, glm::vec2& currentPos, glm::vec2& velocity, double timeStep);

	void ResolvePenetration(const std::vector<GameObject>* blocks, glm::vec2& position, glm::vec2& velocity, glm::vec2 size);

	glm::vec2 testV{ 0.0f };
	PhysicsState mInterpolatedState;

	glm::vec2 pos = glm::vec2(0.0f, 800.0f);

	bool testButton1{ false };
	bool testButton2{ false };
	bool testButton3{ false };
	bool testButton4{ false };

	glm::vec2 accF{ 0.0f };

	glm::vec2 testPos{ 0.0f };	
	
	std::vector<glm::vec2> collBlocks;

private:
	double time = 0.0f;
	double accumulator = 0.0f;
	double timeStep = 1.0f / 128.0f;

	PhysicsState previous;
	PhysicsState current;

	glm::vec2 normal;
	glm::vec2 fric;


	glm::vec2 colAcc{ 0.0f };

};

