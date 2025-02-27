#include "PhysicsComponent.hpp"

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

glm::vec2 PhysicsComponent::MultiplyVec2AndDouble(glm::vec2 vec, double d) const {
	return glm::vec2(vec.x * d, vec.y * d);
}

Derivative PhysicsComponent::RK4_EvaluateVec2(PhysicsState& initial, double time, double timeStep, const Derivative& d) {
	PhysicsState state;

	state.position = initial.position + MultiplyVec2AndDouble(d.PositionDerivative, timeStep);
	state.velocity = initial.velocity + MultiplyVec2AndDouble(d.VelocityDerivative, timeStep);

	Derivative output;
	output.PositionDerivative = state.velocity;
	output.VelocityDerivative = RK4_AccelerationVec2(state, timeStep);


	return output;
}

glm::vec2 PhysicsComponent::RK4_AccelerationVec2(const PhysicsState& state, double time) {
	//glm::vec2 nextPos = state.position + state.velocity;

	//return nextPos - state.position;



	return glm::vec2(0.0f, 0.0f);
}

void PhysicsComponent::RK4_IntegrateVec2(PhysicsState& state, double time, double timeStep) {
	Derivative a, b, c, d;

	a = RK4_EvaluateVec2(state, time, 0.0f, Derivative());
	b = RK4_EvaluateVec2(state, time, timeStep * 0.5f, a);
	c = RK4_EvaluateVec2(state, time, timeStep * 0.5f, b);
	d = RK4_EvaluateVec2(state, time, timeStep, c);

	glm::vec2 positionChange = 1.0f / 6.0f * (a.PositionDerivative + 2.0f * (b.PositionDerivative + c.PositionDerivative) + d.PositionDerivative);
	glm::vec2 velocityChange = 1.0f / 6.0f * (a.VelocityDerivative + 2.0f * (b.VelocityDerivative + c.VelocityDerivative) + d.VelocityDerivative);

	state.position = state.position + MultiplyVec2AndDouble(positionChange, timeStep);
	state.velocity = state.velocity + MultiplyVec2AndDouble(velocityChange, timeStep);

}

void PhysicsComponent::FixedTickrateUpdate(double deltaTime) {

	static float ddd = 0;

	accumulator += deltaTime;

	static double time1 = 0.0f;
	static double accumulator1 = 0.0f;
	static double times = 1.0f / 128.0f;

	accumulator1 += deltaTime;

	while (accumulator1 >= times) {

		ddd -= 10.0f;

		time1 += times;
		accumulator1 -= times;
	}

	while (accumulator >= timeStep) {



		//current.velocity = glm::vec2(1000.0f, -1000.0f);
		previous = current;
		RK4_IntegrateVec2(current, time, timeStep);

		time += timeStep;
		accumulator -= timeStep;

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

		// interpolation is the problem, and dumb future.position.y >= 0 check, the lower the framerate, the lower the interpolated points amount, it chooses closest point, but not specifically that is y = 0
		// its fine, but need to remember to perform all physics here. interpolated position is only for render
	}

	std::cout << glm::to_string(current.velocity) << std::endl;

	const double alpha = accumulator / timeStep;

	mInterpolatedState.position = MultiplyVec2AndDouble(current.position, alpha) + MultiplyVec2AndDouble(previous.position, (1.0f - alpha));
	mInterpolatedState.velocity = MultiplyVec2AndDouble(current.velocity, alpha) + MultiplyVec2AndDouble(previous.velocity, (1.0f - alpha));

	//std::cout << glm::to_string(tes) << std::endl;

	//std::cout << glm::to_string(future.position) << " " << glm::to_string(future.velocity) << std::endl;

	//std::cout << glm::to_string(current.position) << " " << glm::to_string(current.velocity) << std::endl;


	if (mInterpolatedState.position.y >= 0) {

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

		//std::cout << mInterpolatedState.position.x + mInterpolatedState.position.y << std::endl;

		ddd = 0;
		iterations++;
		current.position = glm::vec2(0.0f, 800.0f);
	}

	//std::cout << "pos: " << future.position.x <<  " LowestPos: " << lowest << " HighestPos: " << highest << " Iterations: " << iterations << " DeltaPos: " << highest - lowest << std::endl;


}

void PhysicsComponent::nulify() {
	current.position = glm::vec2(300.0f, 300.0f);

	//lowest = 2000.0f;
	//highest = 0.0f;
	//
	//iterations = 0;
	//
	//vel = glm::vec2(0.0f);
	//pos = glm::vec2(0.0f, 800.0f);
	//
	//lowest1 = 2000.0f;
	//highest1 = 0.0f;
	//
	//iterations1 = 0;
}
