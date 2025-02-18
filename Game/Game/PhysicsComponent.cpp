#include "PhysicsComponent.hpp"
#include <iostream>
#include <chrono>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

static float lowest = 2000.0f;
static float highest = 0.0f;

static int iterations = 0;

static float lowest1 = 2000.0f;
static float highest1 = 0.0f;

static int iterations1 = 0;

static glm::vec2 vel = glm::vec2(0.0f);

PhysicsComponent::PhysicsComponent() {

}

PhysicsComponent::~PhysicsComponent() {

}

Derivative PhysicsComponent::Evaluate2v(State& initial, double time, float timeStep, const Derivative& d) {
	State state;

	state.position = initial.position + d.dPosition * timeStep;
	state.velocity = initial.velocity + d.dVelocity * timeStep;

	Derivative output;
	output.dPosition = state.velocity;
	output.dVelocity = Acceleration2v(state, time + timeStep);
	return output;
}

glm::vec2 PhysicsComponent::Acceleration2v(const State& state, double time) {
	glm::vec2 nextPos = state.position + state.velocity;
	return nextPos - state.position;
}

void PhysicsComponent::Integrate2v(State& state, double time, float timeStep) {
	Derivative a, b, c, d;

	a = Evaluate2v(state, time, 0.0f, Derivative());
	b = Evaluate2v(state, time, timeStep * 0.5f, a);
	c = Evaluate2v(state, time, timeStep * 0.5f, b);
	d = Evaluate2v(state, time, timeStep, c);

	glm::vec2 positionChange = 1.0f / 6.0f * (a.dPosition + 2.0f * (b.dPosition + c.dPosition) + d.dPosition);
	glm::vec2 velocityChange = 1.0f / 6.0f * (a.dVelocity + 2.0f * (b.dVelocity + c.dVelocity) + d.dVelocity);

	state.position = state.position + positionChange * timeStep;
	state.velocity = state.velocity + velocityChange * timeStep;
}

void PhysicsComponent::FixedTickrateUpdate() {

	static auto lastTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();


	static int add = 0;

	if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count() >= 1) {
		//std::cout << add << std::endl;
		add = 0;
		lastTime = currentTime;
	}

	//current.velocity = testV;
	//std::cout << testV.y << std::endl;

	while (mAccumulator >= timeStep) {
		current.velocity = glm::vec2(1000.0f, -1000.0f);
		previous = current;
		Integrate2v(current, time, timeStep);
		time += timeStep;
		mAccumulator -= timeStep;

		add++;

		// test Euler and verlet here to differentiate the root cause of inconsistence 
		// some weird accumulation in vec2 velocity future
		// probuably solved
		// some weird inconsistency with the velocity, instead of 10 its 10 with something or 11, need to investigate
		// there is some fps dependance still, fuck, may be realatet to this ^^
		// need a relibable way to check loop stability and consistancy
		// may be releated to main dt being a float
		// better, not ideal
		// non ideal frame time, it absolutly looses it lol
		// ja ebal rot suka blyad nahoj
		// velocity accumulation, WHAT!?

		// interpolation is the problem, and dumb future.position.y >= 0 check, the lower the framerate, the lower the interpolated points, it chooses closest point, but not specifically that is y = 0
		// its fine, but need to remember to perform all physics here. interpolated position is only for render

		if (pos.y >= 0) {
			// semi-implicit euler
			//vel += acceleration * deltaTime;
			//pos += vel * deltaTime;

			vel += glm::vec2(100.0f, -80.0f);
			pos += vel;
			//std::cout << "Aa" << std::endl;

			// velocity verlet
			//pos += vel * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;
			//vel += acceleration * deltaTime;
		} else {
			if (pos.x > highest1) {
				highest1 = pos.x;
			}
			if (pos.x < lowest1) {
				lowest1 = pos.x;
			}
			iterations1++;

			//std::cout << "pos: " << pos.x << " LowestPos: " << lowest1 << " HighestPos: " << highest1 << " Iterations: " << iterations1 << " DeltaPos: " << highest1 - lowest1 << std::endl;

			pos = glm::vec2(0.0f, 800.0f);
			vel = glm::vec2(0.0f);
		}
	}


	const double alpha = mAccumulator / timeStep;

	future.position = glm::vec2(0.0f);
	future.velocity = glm::vec2(0.0f);


	future.position = glm::vec2(current.position.x * alpha, current.position.y * alpha) + glm::vec2(previous.position.x * (1.0f - alpha), previous.position.y * (1.0f - alpha));
	future.velocity = glm::vec2(current.velocity.x * alpha, current.velocity.y * alpha) + glm::vec2(previous.velocity.x * (1.0f - alpha), previous.velocity.y * (1.0f - alpha));

	glm::vec2 tes = glm::vec2(current.velocity.x, current.velocity.y) + glm::vec2(previous.velocity.x, previous.velocity.y);

	//std::cout << glm::to_string(tes) << std::endl;

	//std::cout << glm::to_string(future.position) << " " << glm::to_string(future.velocity) << std::endl;

	//std::cout << glm::to_string(current.position) << " " << glm::to_string(current.velocity) << std::endl;


	if (future.position.y >= 0) {

	}
	else {
		//if (future.position.x > highest) {
		//	highest = future.position.x;
		//}
		//if (future.position.x < lowest) {
		//	lowest = future.position.x;
		//}
		//std::cout << "pos: " << future.position.x << " LowestPos: " << lowest << " HighestPos: " << highest << " Iterations: " << iterations << " DeltaPos: " << highest - lowest << std::endl;

		//std::cout << glm::to_string(future.position) << " " << glm::to_string(future.velocity) << std::endl;

		//std::cout << future.position.x + future.position.y << std::endl;

		iterations++;
		current.position = glm::vec2(0.0f, 800.0f);
	}

	//std::cout << "pos: " << future.position.x <<  " LowestPos: " << lowest << " HighestPos: " << highest << " Iterations: " << iterations << " DeltaPos: " << highest - lowest << std::endl;
	

}

void PhysicsComponent::nulify() {
	current.position = glm::vec2(0.0f, 800.0f);

	lowest = 2000.0f;
	highest = 0.0f;

	iterations = 0;

	vel = glm::vec2(0.0f);
	pos = glm::vec2(0.0f, 800.0f);

	lowest1 = 2000.0f;
	highest1 = 0.0f;

	iterations1 = 0;
}
