#pragma once
#include <glm/glm.hpp>


struct State
{
	glm::vec2 position;
	glm::vec2 velocity;

	float x;      // position
	float v;      // velocity
};

struct Derivative
{
	glm::vec2 dPosition;
	glm::vec2 dVelocity;

	float dx;      // dx/dt = velocity
	float dv;      // dv/dt = acceleration
};

class PhysicsComponent {
public:
	PhysicsComponent();

	~PhysicsComponent();

	Derivative Evaluate2v(State& initial, double time, float timeStep, const Derivative& d);

	glm::vec2 Acceleration2v(const State& state, double time);

	void Integrate2v(State& state, double time, float timeStep);

	void FixedTickrateUpdate();

	glm::vec2 testV{ 0.0f };
	State future;
	double mAccumulator = 0.0f;

	void nulify();

	glm::vec2 pos = glm::vec2(0.0f, 800.0f);

private:
	double time = 0.0f;
	double timeStep = 1.0f / 128.0f;

	State previous;
	State current;
};

