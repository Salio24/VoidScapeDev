#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <chrono>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

struct PhysicsState
{
	glm::vec2 position;
	glm::vec2 velocity;
};

struct Derivative
{
	glm::vec2 PositionDerivative;
	glm::vec2 VelocityDerivative;
};

class PhysicsComponent {
public:
	PhysicsComponent();

	~PhysicsComponent();

	glm::vec2 MultiplyVec2AndDouble(glm::vec2 vec, double d) const;

	Derivative RK4_EvaluateVec2(PhysicsState& initial, double time, double timeStep, const Derivative& d);

	glm::vec2 RK4_AccelerationVec2(const PhysicsState& state, double time);

	void RK4_IntegrateVec2(PhysicsState& state, double time, double timeStep);

	void FixedTickrateUpdate(double deltaTime);

	glm::vec2 testV{ 0.0f };
	PhysicsState mInterpolatedState;

	void nulify();

	glm::vec2 pos = glm::vec2(0.0f, 800.0f);

private:
	double time = 0.0f;
	double accumulator = 0.0f;
	double timeStep = 1.0f / 128.0f;

	PhysicsState previous;
	PhysicsState current;
};

