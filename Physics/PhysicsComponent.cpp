#include "PhysicsComponent.hpp"
#include <set>

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

	glm::vec2 acc = glm::vec2(0.0f);

	if (testButton3) {
		acc.y -= 1000.0f;
	}
	if (testButton4) {
		acc.y += 3000.0f;
	}

	return acceleration + acc;
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

void PhysicsComponent::FixedTickrateUpdate(double deltaTime, const std::vector<GameObject>* blocks, bool activeKeys[static_cast<int>(ActiveKeys::DUCK)]) {

	// add 2 distinct structs, static box and dynamic box

	accumulator += deltaTime;

	while (accumulator >= timeStep) {






		previous = current;
		MovementUpdate(activeKeys);
		RK4_IntegrateVec2(current, time, timeStep);
		CollisionUpdate(blocks, previous.position, current.position, current.velocity, timeStep);

	
		//std::cout << glm::to_string(current.velocity) << glm::to_string(current.position) << glm::to_string(acceleration) << glm::to_string(normal) << glm::to_string(frictest) << std::endl;

		if (current.position.y > 472.83f) {
			std::cout << "Velocity: " << glm::to_string(current.velocity) << " Position: " << glm::to_string(current.position) << " Acceleration: " << glm::to_string(acceleration) << " Delta vel: " << current.velocity.y - previous.velocity.y << " Delta pos: " << current.position.y - previous.position.y << std::endl;
		}


		time += timeStep;
		accumulator -= timeStep;
	}


	const double alpha = accumulator / timeStep;

	mInterpolatedState.position = MultiplyVec2AndDouble(current.position, alpha) + MultiplyVec2AndDouble(previous.position, (1.0f - alpha));
	mInterpolatedState.velocity = MultiplyVec2AndDouble(current.velocity, alpha) + MultiplyVec2AndDouble(previous.velocity, (1.0f - alpha));



	//std::cout << glm::to_string(normal) << std::endl;
}

bool PhysicsComponent::PointVsRect(const glm::vec2& point, const glm::vec2& boxSize, const glm::vec2& boxPos) {
	return (point.x >= boxPos.x && point.y >= boxPos.y && point.x < boxPos.x + boxSize.x && point.y < boxPos.y + boxSize.y);
}

bool PhysicsComponent::RectVsRect(const glm::vec2 rect1Pos, const glm::vec2 rect1Size, const glm::vec2 rect2Pos, const glm::vec2 rect2Size) {
	return (rect1Pos.x < rect2Pos.x + rect2Size.x && rect1Pos.x + rect1Size.x > rect2Pos.x && rect1Pos.y < rect2Pos.y + rect2Size.y && rect1Pos.y + rect1Size.y > rect2Pos.y);
}


bool PhysicsComponent::RayVsRect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const Box* target, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear)
{
	contactNormal = { 0,0 };
	contactPoint = { 0,0 };

	// Cache division
	glm::vec2 invDirection = 1.0f / rayDirection;

	// Calculate intersections with rectangle bounding axes
	glm::vec2 timeNear = (target->Position - rayOrigin) * invDirection;
	glm::vec2 timeFar = (target->Position + target->Size - rayOrigin) * invDirection;

	if (std::isnan(timeFar.y) || std::isnan(timeFar.x)) {
		return false;
	}
	if (std::isnan(timeNear.y) || std::isnan(timeNear.x)) {
		return false;
	}

	// Sort distances
	if (timeNear.x > timeFar.x) {
		std::swap(timeNear.x, timeFar.x);
	}
	if (timeNear.y > timeFar.y) {
		std::swap(timeNear.y, timeFar.y);
	}

	// Early rejection		
	if (timeNear.x > timeFar.y || timeNear.y > timeFar.x) {
		return false;
	}

	// Closest 'time' will be the first contact
	hitTimeNear = std::max(timeNear.x, timeNear.y);

	// Furthest 'time' is contact on opPositionite side of target
	float hitTimeFar = std::min(timeFar.x, timeFar.y);

	// Reject if ray direction is pointing away from object
	if (hitTimeFar < 0) {
		return false;
	}

	// Contact point of collision from parametric line equation
	contactPoint = rayOrigin + hitTimeNear * rayDirection;

	if (timeNear.x > timeNear.y) {
		if (invDirection.x < 0) {
			contactNormal = { 1, 0 };
		}
		else {
			contactNormal = { -1, 0 };
		}
	}
	else if (timeNear.x < timeNear.y) {
		if (invDirection.y < 0) {
			contactNormal = { 0, 1 };
		}
		else {
			contactNormal = { 0, -1 };
		}
	}
	return true;
}

void PhysicsComponent::CollisionUpdate(const std::vector<GameObject>* blocks, glm::vec2 previousPos, glm::vec2& currentPos, glm::vec2& velocity, double timeStep) {

	ResolvePenetration(blocks, currentPos, velocity, glm::vec2(40.0f));

	if (normal.y == 1) {
		mGrounded = true;
	}
	else {
		mGrounded = false;
	}
}

//vec2(-494.889832, -1065.360596)vec2(927.416077, 288.115143)vec2(0.000000, 0.000000)vec2(0.000000, 0.000000)
//vec2(-493.902435, -1065.437256)vec2(925.768921, 284.568420)vec2(0.000000, 0.000000)vec2(0.000000, 0.000000)
//vec2(-493.902435, -1065.437256)vec2(924.120483, 281.016083)vec2(0.000000, 0.000000)vec2(0.000000, 0.000000)
//vec2(-492.916718, -1065.513916)vec2(922.474365, 277.466736)vec2(0.000000, 0.000000)vec2(0.000000, 0.000000)
//vec2(-492.916718, -1065.513916)vec2(920.818359, 273.890778)vec2(0.000000, 0.000000)vec2(0.000000, 0.000000)
//vec2(-492.916718, -1065.513916)vec2(919.184692, 270.362885)vec2(0.000000, 0.000000)vec2(0.000000, 0.000000)
//vec2(0.000000, 0.000000)vec2(918.645630, 270.079895)vec2(1.000000, 1.000000)vec2(0.000000, 0.000000)
//vec2(0.000000, 0.000000)vec2(918.188110, 270.023285)vec2(1.000000, 1.000000)vec2(0.000000, 0.000000)
//vec2(0.000000, 0.000000)vec2(918.000000, 270.000000)vec2(0.000000, 1.000000)vec2(0.000000, 0.000000)
//vec2(0.000000, 0.000000)vec2(918.000000, 270.000000)vec2(0.000000, 1.000000)vec2(0.000000, 0.000000)
//vec2(0.000000, 0.000000)vec2(918.000000, 270.000000)vec2(0.000000, 1.000000)vec2(0.000000, 0.000000)



//vec2(-388.555298, -907.268372)vec2(905.328857, 277.570221)vec2(0.000000, 0.000000)vec2(0.000000, 0.000000)
//vec2(-388.555298, -907.268372)vec2(904.033203, 274.548798)vec2(0.000000, 0.000000)vec2(0.000000, 0.000000)
//
//vec2(1.000000, 0.000000)Ovx: 0.0968018, Ovy : 5.10031 MinA : vec2(899.903198, 264.899689)MaxA : vec2(939.903198, 304.899689)MinB : vec2(882.000000, 252.000000)MaxB : vec2(900.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : 18, Ovy : 5.10031 MinA : vec2(900.000000, 264.899689)MaxA : vec2(940.000000, 304.899689)MinB : vec2(900.000000, 252.000000)MaxB : vec2(918.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : 18, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(918.000000, 252.000000)MaxB : vec2(936.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : 4, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(936.000000, 252.000000)MaxB : vec2(954.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : -14, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(954.000000, 252.000000)MaxB : vec2(972.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : -32, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(972.000000, 252.000000)MaxB : vec2(990.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : -50, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(990.000000, 252.000000)MaxB : vec2(1008.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : -68, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(1008.000000, 252.000000)MaxB : vec2(1026.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : -86, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(1026.000000, 252.000000)MaxB : vec2(1044.000000, 270.000000)
//vec2(1.000000, 1.000000)Ovx : -104, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(1044.000000, 252.000000)MaxB : vec2(1062.000000, 270.000000)
//
//vec2(0.000000, 0.000000)vec2(902.745117, 271.859314)vec2(1.000000, 1.000000)vec2(0.000000, 0.000000)
//
//vec2(0.000000, 1.000000)Ovx : 18, Ovy : 0.000305176 MinA : vec2(900.000000, 269.999695)MaxA : vec2(940.000000, 309.999695)MinB : vec2(900.000000, 252.000000)MaxB : vec2(918.000000, 270.000000)
//vec2(0.000000, 1.000000)Ovx : 18, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(918.000000, 252.000000)MaxB : vec2(936.000000, 270.000000)
//vec2(0.000000, 1.000000)Ovx : 4, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(936.000000, 252.000000)MaxB : vec2(954.000000, 270.000000)
//vec2(0.000000, 1.000000)Ovx : -14, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(954.000000, 252.000000)MaxB : vec2(972.000000, 270.000000)
//vec2(0.000000, 1.000000)Ovx : -32, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(972.000000, 252.000000)MaxB : vec2(990.000000, 270.000000)
//vec2(0.000000, 1.000000)Ovx : -50, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(990.000000, 252.000000)MaxB : vec2(1008.000000, 270.000000)
//vec2(0.000000, 1.000000)Ovx : -68, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(1008.000000, 252.000000)MaxB : vec2(1026.000000, 270.000000)
//vec2(0.000000, 1.000000)Ovx : -86, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(1026.000000, 252.000000)MaxB : vec2(1044.000000, 270.000000)
//vec2(0.000000, 1.000000)Ovx : -104, Ovy : 0 MinA : vec2(900.000000, 270.000000)MaxA : vec2(940.000000, 310.000000)MinB : vec2(1044.000000, 252.000000)MaxB : vec2(1062.000000, 270.000000)
//
//vec2(0.000000, 0.000000)vec2(900.000000, 270.000000)vec2(0.000000, 1.000000)vec2(0.000000, 0.000000)



void PhysicsComponent::ResolvePenetration(const std::vector<GameObject>* blocks, glm::vec2& position, glm::vec2& velocity, glm::vec2 size) {

	normal = glm::vec2(0.0f);

	// Check penetration on high speeds / low tick rate, i suspect it might fail if overlap is bigger than half of the block
	// Add support for sliding mechanic (under the blocks)

	
	// a possible fix is to assure the players y pos is < than blocks t pos + y size, for collision on x to occur
	// same with y axis

	// will deal with it later, fuck it 

	int asd = 0;

		// Bottom left corner of broad-phase-box
	glm::vec2 A(position.x - 3 * 40.0f, position.y - 3 * 40.0f);
	// Top right corner of broad-phase-box
	glm::vec2 B(position.x + 4 * 40.0f, position.y + 4 * 40.0f);

	for (const auto& block : *blocks) {
		if (!block.mIsDeathTrigger && block.mIsCollidable && block.mSprite.mVertexData.Position.x > A.x && block.mSprite.mVertexData.Position.x < B.x && block.mSprite.mVertexData.Position.y > A.y && block.mSprite.mVertexData.Position.y < B.y) {

			// AABB bounds: character
			glm::vec2 minA = position;
			glm::vec2 maxA = position + size;
			// AABB bounds: static block
			glm::vec2 minB = block.mSprite.mVertexData.Position;
			glm::vec2 maxB = minB + block.mSprite.mVertexData.Size;

			// Calculate overlap in each axis
			float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
			float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);

			if (overlapX > 0 && overlapY > 0) {
				// Resolve along the axis with the smallest penetration
				if (overlapX < overlapY) {
					// Resolve along X-axis
					if (minA.x < minB.x) {
						position.x -= overlapX; // Move left
						normal.x = -1.0f;
						if (velocity.x > 0) velocity.x = 0; 
					}
					else {
						position.x += overlapX; // Move right
						normal.x = 1.0f;
						if (velocity.x < 0) velocity.x = 0; 
					}
				}
				else {
					// Resolve along Y-axis
					if (minA.y < minB.y) {
						position.y -= overlapY; // Move down 
						normal.y = -1.0f;
						if (velocity.y > 0) velocity.y = 0;
					}
					else {
						position.y += overlapY; // Move up
						normal.y = 1.0f;
						if (velocity.y < 0) velocity.y = 0; 
					}
				}
			}
			if (normal.x != 0.0f || normal.y != 0.0f) {
				//std::cout << glm::to_string(normal) << "Ovx: " << overlapX << ", Ovy: " << overlapY << "MinA: " << glm::to_string(minA) << "MaxA: " << glm::to_string(maxA) << "MinB: " << glm::to_string(minB) << "MaxB: " << glm::to_string(maxB) << std::endl;
			}
			asd++;
		}
	}
			//std::cout << asd << std::endl;

	// add trigger handling, 
}

void PhysicsComponent::MovementUpdate(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)]) {
	acceleration = glm::vec2(0.0f, 0.0f);

	// Gravity
	if (current.velocity.y <= 0 && current.velocity.y > mPhysicsSettings.FallSpeedLimit) {
		acceleration.y += mPhysicsSettings.GravityDOWN;
	}
	else if (current.velocity.y > 0) {
		acceleration.y += mPhysicsSettings.GravityUP;
	}

	// Fall speed limit
	if (current.velocity.y < mPhysicsSettings.FallSpeedLimit) {
		acceleration.y += -(current.velocity.y - mPhysicsSettings.FallSpeedLimit) / timeStep;
	}

	// Movement on X-axis (run) on ground and mid air
	if (activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)]) {
		if (current.velocity.x > -mPhysicsSettings.RunSpeedLimit) {
			if (mGrounded) {
				if (current.velocity.x - mPhysicsSettings.RunAccelerationOnFoot * timeStep >= -mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += -mPhysicsSettings.RunAccelerationOnFoot;
				}
				else if (current.velocity.x < 0.0f) {
					acceleration.x += -(current.velocity.x + mPhysicsSettings.RunSpeedLimit) / timeStep;
				}
			}
			else {
				if (current.velocity.x - mPhysicsSettings.RunAccelerationMidAir * timeStep >= -mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += -mPhysicsSettings.RunAccelerationMidAir;
				}
				else if (current.velocity.x < 0.0f) {
					acceleration.x += -(current.velocity.x + mPhysicsSettings.RunSpeedLimit) / timeStep;
				}
			}
		}
	}
	if (activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) {
		if (current.velocity.x < mPhysicsSettings.RunSpeedLimit) {
			if (mGrounded) {
				if (current.velocity.x + mPhysicsSettings.RunAccelerationOnFoot * timeStep <= mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += mPhysicsSettings.RunAccelerationOnFoot;
				}
				else if (current.velocity.x > 0.0f) {
					acceleration.x += -(current.velocity.x - mPhysicsSettings.RunSpeedLimit) / timeStep;
				}
			}
			else {
				if (current.velocity.x + mPhysicsSettings.RunAccelerationMidAir * timeStep <= mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += mPhysicsSettings.RunAccelerationMidAir;
				}
				else if (current.velocity.x > 0.0f) {
					acceleration.x += -(current.velocity.x - mPhysicsSettings.RunSpeedLimit) / timeStep;
				}
			}
		}
	}

	static bool isJumping = false;

	static int ticks = 0;

	// Test
	if (activeKeys[static_cast<int>(ActiveKeys::DUCK)]) {
		//acceleration.y -= 1000.0f;
	}


	// Friction on gound and air resistance mid air
	if (!(activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)] || activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) && std::abs(current.velocity.x) > 0.0f) {
		if (mGrounded) { // On ground
			if (std::abs(current.velocity.x) < mPhysicsSettings.FrictionStopSpeed) {
				// No input: stop the object when velocity is below threshold
				acceleration.x = -current.velocity.x / timeStep;
			}
			else {
				// Moving without input: apply constant friction	
				acceleration.x = -mPhysicsSettings.DefaultFriction * glm::normalize(current.velocity).x;
				// values > ~3800.0f are unstable 
			}
		}
		else {
			if (std::abs(current.velocity.x) < mPhysicsSettings.AirResistanceStopSpeed) {
				// No input: stop the object when velocity is below threshold
				acceleration.x = -current.velocity.x / timeStep;
			}
			else {
				// Moving without input: apply constant friction
				acceleration.x = -mPhysicsSettings.DefaultAirResistance * glm::normalize(current.velocity).x;
				// values > ~3800.0f are unstable
			}
		}
	}


	if (activeKeys[static_cast<int>(ActiveKeys::SPACE)]) {
		if (mSpacebarOneShot) {

			jumpBufferTimer = std::chrono::high_resolution_clock::now();



			mSpacebarOneShot = false;
		}



		if (isJumping && ticks < 64) {
			ticks++;
			acceleration.y += 2000.0f;
			//std::cout << "a" << std::endl;
		}


	}

	if (mGrounded && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpBufferTimer).count() < 200) {
		jumpBufferTimer = std::chrono::time_point<std::chrono::steady_clock>{};
		isJumping = true;

		ticks = 0;

		acceleration.y += mPhysicsSettings.JumpStartImpulse;
		mGrounded = false;
		//std::cout << "bv" << std::endl;
	}
	if (mGrounded || current.velocity.y < 0.0f) {
		isJumping = false;
	}

	// 202.849945

}

/*
Velocity: vec2(0.000000, 190.625000) Position: vec2(1058.000000, 270.372314) Acceelration: vec2(0.000000, 48800.000000)
Velocity: vec2(0.000000, 194.921875) Position: vec2(1058.000000, 271.125336) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 199.218750) Position: vec2(1058.000000, 271.895142) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 203.515625) Position: vec2(1058.000000, 272.681732) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 207.812500) Position: vec2(1058.000000, 273.485107) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 212.109375) Position: vec2(1058.000000, 274.305267) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 216.406250) Position: vec2(1058.000000, 275.142212) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 220.703125) Position: vec2(1058.000000, 275.995941) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 225.000000) Position: vec2(1058.000000, 276.866455) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 229.296875) Position: vec2(1058.000000, 277.753754) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 233.593750) Position: vec2(1058.000000, 278.657837) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 237.890625) Position: vec2(1058.000000, 279.578705) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 242.187500) Position: vec2(1058.000000, 280.516357) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 246.484375) Position: vec2(1058.000000, 281.470795) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 250.781250) Position: vec2(1058.000000, 282.442017) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 255.078125) Position: vec2(1058.000000, 283.430023) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 259.375000) Position: vec2(1058.000000, 284.434814) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 263.671875) Position: vec2(1058.000000, 285.456390) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 267.968750) Position: vec2(1058.000000, 286.494751) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 272.265625) Position: vec2(1058.000000, 287.549896) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 276.562500) Position: vec2(1058.000000, 288.621826) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 280.859375) Position: vec2(1058.000000, 289.710541) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 285.156250) Position: vec2(1058.000000, 290.816040) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 289.453125) Position: vec2(1058.000000, 291.938324) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 293.750000) Position: vec2(1058.000000, 293.077393) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 298.046875) Position: vec2(1058.000000, 294.233246) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 302.343750) Position: vec2(1058.000000, 295.405884) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 306.640625) Position: vec2(1058.000000, 296.595306) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 310.937500) Position: vec2(1058.000000, 297.801514) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 315.234375) Position: vec2(1058.000000, 299.024506) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 319.531250) Position: vec2(1058.000000, 300.264282) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 323.828125) Position: vec2(1058.000000, 301.520844) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 328.125000) Position: vec2(1058.000000, 302.794189) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 332.421875) Position: vec2(1058.000000, 304.084320) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 336.718750) Position: vec2(1058.000000, 305.391235) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 341.015625) Position: vec2(1058.000000, 306.714935) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 345.312500) Position: vec2(1058.000000, 308.055420) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 349.609375) Position: vec2(1058.000000, 309.412689) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 353.906250) Position: vec2(1058.000000, 310.786743) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 358.203125) Position: vec2(1058.000000, 312.177582) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 362.500000) Position: vec2(1058.000000, 313.585205) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 366.796875) Position: vec2(1058.000000, 315.009613) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 371.093750) Position: vec2(1058.000000, 316.450806) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 375.390625) Position: vec2(1058.000000, 317.908783) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 379.687500) Position: vec2(1058.000000, 319.383545) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 383.984375) Position: vec2(1058.000000, 320.875092) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 388.281250) Position: vec2(1058.000000, 322.383423) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 392.578125) Position: vec2(1058.000000, 323.908539) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 396.875000) Position: vec2(1058.000000, 325.450439) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 401.171875) Position: vec2(1058.000000, 327.009125) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 405.468750) Position: vec2(1058.000000, 328.584595) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 409.765625) Position: vec2(1058.000000, 330.176849) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 414.062500) Position: vec2(1058.000000, 331.785889) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 418.359375) Position: vec2(1058.000000, 333.411713) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 422.656250) Position: vec2(1058.000000, 335.054321) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 426.953125) Position: vec2(1058.000000, 336.713715) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 431.250000) Position: vec2(1058.000000, 338.389893) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 435.546875) Position: vec2(1058.000000, 340.082855) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 439.843750) Position: vec2(1058.000000, 341.792603) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 444.140625) Position: vec2(1058.000000, 343.519135) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 448.437500) Position: vec2(1058.000000, 345.262451) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 452.734375) Position: vec2(1058.000000, 347.022552) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 457.031250) Position: vec2(1058.000000, 348.799438) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 461.328125) Position: vec2(1058.000000, 350.593109) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 465.625000) Position: vec2(1058.000000, 352.403564) Acceelration: vec2(0.000000, 1100.000000)
Velocity: vec2(0.000000, 462.109375) Position: vec2(1058.000000, 354.215546) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 458.593750) Position: vec2(1058.000000, 356.013794) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 455.078125) Position: vec2(1058.000000, 357.798309) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 451.562500) Position: vec2(1058.000000, 359.569092) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 448.046875) Position: vec2(1058.000000, 361.326141) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 444.531250) Position: vec2(1058.000000, 363.069458) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 441.015625) Position: vec2(1058.000000, 364.799042) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 437.500000) Position: vec2(1058.000000, 366.514893) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 433.984375) Position: vec2(1058.000000, 368.217010) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 430.468750) Position: vec2(1058.000000, 369.905396) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 426.953125) Position: vec2(1058.000000, 371.580048) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 423.437500) Position: vec2(1058.000000, 373.240967) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 419.921875) Position: vec2(1058.000000, 374.888153) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 416.406250) Position: vec2(1058.000000, 376.521606) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 412.890625) Position: vec2(1058.000000, 378.141327) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 409.375000) Position: vec2(1058.000000, 379.747314) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 405.859375) Position: vec2(1058.000000, 381.339569) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 402.343750) Position: vec2(1058.000000, 382.918091) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 398.828125) Position: vec2(1058.000000, 384.482880) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 395.312500) Position: vec2(1058.000000, 386.033936) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 391.796875) Position: vec2(1058.000000, 387.571259) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 388.281250) Position: vec2(1058.000000, 389.094849) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 384.765625) Position: vec2(1058.000000, 390.604706) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 381.250000) Position: vec2(1058.000000, 392.100830) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 377.734375) Position: vec2(1058.000000, 393.583221) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 374.218750) Position: vec2(1058.000000, 395.051880) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 370.703125) Position: vec2(1058.000000, 396.506805) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 367.187500) Position: vec2(1058.000000, 397.947998) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 363.671875) Position: vec2(1058.000000, 399.375458) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 360.156250) Position: vec2(1058.000000, 400.789185) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 356.640625) Position: vec2(1058.000000, 402.189178) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 353.125000) Position: vec2(1058.000000, 403.575439) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 349.609375) Position: vec2(1058.000000, 404.947968) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 346.093750) Position: vec2(1058.000000, 406.306763) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 342.578125) Position: vec2(1058.000000, 407.651825) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 339.062500) Position: vec2(1058.000000, 408.983154) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 335.546875) Position: vec2(1058.000000, 410.300751) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 328.515625) Position: vec2(1058.000000, 412.894745) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 325.000000) Position: vec2(1058.000000, 414.171143) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 332.031250) Position: vec2(1058.000000, 411.604614) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 321.484375) Position: vec2(1058.000000, 415.433807) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 317.968750) Position: vec2(1058.000000, 416.682739) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 314.453125) Position: vec2(1058.000000, 417.917938) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 310.937500) Position: vec2(1058.000000, 419.139404) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 307.421875) Position: vec2(1058.000000, 420.347137) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 303.906250) Position: vec2(1058.000000, 421.541138) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 300.390625) Position: vec2(1058.000000, 422.721405) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 296.875000) Position: vec2(1058.000000, 423.887939) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 293.359375) Position: vec2(1058.000000, 425.040741) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 289.843750) Position: vec2(1058.000000, 426.179810) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 286.328125) Position: vec2(1058.000000, 427.305145) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 282.812500) Position: vec2(1058.000000, 428.416748) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 279.296875) Position: vec2(1058.000000, 429.514618) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 275.781250) Position: vec2(1058.000000, 430.598755) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 272.265625) Position: vec2(1058.000000, 431.669159) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 268.750000) Position: vec2(1058.000000, 432.725830) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 265.234375) Position: vec2(1058.000000, 433.768768) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 261.718750) Position: vec2(1058.000000, 434.797974) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 258.203125) Position: vec2(1058.000000, 435.813446) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 254.687500) Position: vec2(1058.000000, 436.815186) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 251.171875) Position: vec2(1058.000000, 437.803192) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 247.656250) Position: vec2(1058.000000, 438.777466) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 244.140625) Position: vec2(1058.000000, 439.738007) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 240.625000) Position: vec2(1058.000000, 440.684814) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 237.109375) Position: vec2(1058.000000, 441.617889) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 233.593750) Position: vec2(1058.000000, 442.537231) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 230.078125) Position: vec2(1058.000000, 443.442841) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 226.562500) Position: vec2(1058.000000, 444.334717) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 223.046875) Position: vec2(1058.000000, 445.212860) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 219.531250) Position: vec2(1058.000000, 446.077271) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 216.015625) Position: vec2(1058.000000, 446.927948) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 212.500000) Position: vec2(1058.000000, 447.764893) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 208.984375) Position: vec2(1058.000000, 448.588104) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 205.468750) Position: vec2(1058.000000, 449.397583) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 201.953125) Position: vec2(1058.000000, 450.193329) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 198.437500) Position: vec2(1058.000000, 450.975342) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 194.921875) Position: vec2(1058.000000, 451.743622) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 191.406250) Position: vec2(1058.000000, 452.498169) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 187.890625) Position: vec2(1058.000000, 453.238983) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 184.375000) Position: vec2(1058.000000, 453.966064) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 180.859375) Position: vec2(1058.000000, 454.679413) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 177.343750) Position: vec2(1058.000000, 455.379028) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 173.828125) Position: vec2(1058.000000, 456.064911) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 170.312500) Position: vec2(1058.000000, 456.737061) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 166.796875) Position: vec2(1058.000000, 457.395477) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 163.281250) Position: vec2(1058.000000, 458.040161) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 159.765625) Position: vec2(1058.000000, 458.671112) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 156.250000) Position: vec2(1058.000000, 459.288330) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 152.734375) Position: vec2(1058.000000, 459.891815) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 149.218750) Position: vec2(1058.000000, 460.481567) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 145.703125) Position: vec2(1058.000000, 461.057587) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 142.187500) Position: vec2(1058.000000, 461.619873) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 138.671875) Position: vec2(1058.000000, 462.168427) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 135.156250) Position: vec2(1058.000000, 462.703247) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 131.640625) Position: vec2(1058.000000, 463.224335) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 128.125000) Position: vec2(1058.000000, 463.731689) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 124.609375) Position: vec2(1058.000000, 464.225311) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 121.093750) Position: vec2(1058.000000, 464.705200) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 117.578125) Position: vec2(1058.000000, 465.171356) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 114.062500) Position: vec2(1058.000000, 465.623779) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 110.546875) Position: vec2(1058.000000, 466.062469) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 107.031250) Position: vec2(1058.000000, 466.487427) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 103.515625) Position: vec2(1058.000000, 466.898651) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 100.000000) Position: vec2(1058.000000, 467.296143) Acceelration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 96.484375) Position: vec2(1058.000000, 467.679901) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 92.968750) Position: vec2(1058.000000, 468.049927) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 89.453125) Position: vec2(1058.000000, 468.406219) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 85.937500) Position: vec2(1058.000000, 468.748779) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 82.421875) Position: vec2(1058.000000, 469.077606) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 78.906250) Position: vec2(1058.000000, 469.392700) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 75.390625) Position: vec2(1058.000000, 469.694061) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 71.875000) Position: vec2(1058.000000, 469.981689) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 68.359375) Position: vec2(1058.000000, 470.255585) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 64.843750) Position: vec2(1058.000000, 470.515747) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 61.328125) Position: vec2(1058.000000, 470.762177) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 57.812500) Position: vec2(1058.000000, 470.994873) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 54.296875) Position: vec2(1058.000000, 471.213837) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 50.781250) Position: vec2(1058.000000, 471.419067) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 47.265625) Position: vec2(1058.000000, 471.610565) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 43.750000) Position: vec2(1058.000000, 471.788330) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 40.234375) Position: vec2(1058.000000, 471.952362) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 36.718750) Position: vec2(1058.000000, 472.102661) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 33.203125) Position: vec2(1058.000000, 472.239227) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 29.687500) Position: vec2(1058.000000, 472.362061) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 26.171875) Position: vec2(1058.000000, 472.471161) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 22.656250) Position: vec2(1058.000000, 472.566528) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 19.140625) Position: vec2(1058.000000, 472.648163) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 15.625000) Position: vec2(1058.000000, 472.716064) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 12.109375) Position: vec2(1058.000000, 472.770233) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 8.593750) Position: vec2(1058.000000, 472.810669) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 5.078125) Position: vec2(1058.000000, 472.837372) Acceleration: vec2(0.000000, -900.000000)
Velocity: vec2(0.000000, 1.562500) Position: vec2(1058.000000, 472.850342) Acceleration: vec2(0.000000, -900.000000)
*/
/*
Velocity: vec2(0.000000, 190.625000) Position : vec2(1058.000000, 270.372314) Acceleration : vec2(0.000000, 48800.000000) Delta vel : 190.625 Delta pos : 0.372314
Velocity : vec2(0.000000, 194.921875) Position : vec2(1058.000000, 271.125336) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.753021
Velocity : vec2(0.000000, 199.218750) Position : vec2(1058.000000, 271.895142) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.769806
Velocity : vec2(0.000000, 203.515625) Position : vec2(1058.000000, 272.681732) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.786591
Velocity : vec2(0.000000, 207.812500) Position : vec2(1058.000000, 273.485107) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.803375
Velocity : vec2(0.000000, 212.109375) Position : vec2(1058.000000, 274.305267) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.82016
Velocity : vec2(0.000000, 216.406250) Position : vec2(1058.000000, 275.142212) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.836945
Velocity : vec2(0.000000, 220.703125) Position : vec2(1058.000000, 275.995941) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.853729
Velocity : vec2(0.000000, 225.000000) Position : vec2(1058.000000, 276.866455) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.870514
Velocity : vec2(0.000000, 229.296875) Position : vec2(1058.000000, 277.753754) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.887299
Velocity : vec2(0.000000, 233.593750) Position : vec2(1058.000000, 278.657837) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.904083
Velocity : vec2(0.000000, 237.890625) Position : vec2(1058.000000, 279.578705) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.920868
Velocity : vec2(0.000000, 242.187500) Position : vec2(1058.000000, 280.516357) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.937653
Velocity : vec2(0.000000, 246.484375) Position : vec2(1058.000000, 281.470795) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.954437
Velocity : vec2(0.000000, 250.781250) Position : vec2(1058.000000, 282.442017) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.971222
Velocity : vec2(0.000000, 255.078125) Position : vec2(1058.000000, 283.430023) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 0.988007
Velocity : vec2(0.000000, 259.375000) Position : vec2(1058.000000, 284.434814) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.00479
Velocity : vec2(0.000000, 263.671875) Position : vec2(1058.000000, 285.456390) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.02158
Velocity : vec2(0.000000, 267.968750) Position : vec2(1058.000000, 286.494751) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.03836
Velocity : vec2(0.000000, 272.265625) Position : vec2(1058.000000, 287.549896) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.05515
Velocity : vec2(0.000000, 276.562500) Position : vec2(1058.000000, 288.621826) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.07193
Velocity : vec2(0.000000, 280.859375) Position : vec2(1058.000000, 289.710541) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.08871
Velocity : vec2(0.000000, 285.156250) Position : vec2(1058.000000, 290.816040) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.1055
Velocity : vec2(0.000000, 289.453125) Position : vec2(1058.000000, 291.938324) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.12228
Velocity : vec2(0.000000, 293.750000) Position : vec2(1058.000000, 293.077393) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.13907
Velocity : vec2(0.000000, 298.046875) Position : vec2(1058.000000, 294.233246) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.15585
Velocity : vec2(0.000000, 302.343750) Position : vec2(1058.000000, 295.405884) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.17264
Velocity : vec2(0.000000, 306.640625) Position : vec2(1058.000000, 296.595306) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.18942
Velocity : vec2(0.000000, 310.937500) Position : vec2(1058.000000, 297.801514) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.20621
Velocity : vec2(0.000000, 315.234375) Position : vec2(1058.000000, 299.024506) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.22299
Velocity : vec2(0.000000, 319.531250) Position : vec2(1058.000000, 300.264282) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.23978
Velocity : vec2(0.000000, 323.828125) Position : vec2(1058.000000, 301.520844) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.25656
Velocity : vec2(0.000000, 328.125000) Position : vec2(1058.000000, 302.794189) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.27335
Velocity : vec2(0.000000, 332.421875) Position : vec2(1058.000000, 304.084320) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.29013
Velocity : vec2(0.000000, 336.718750) Position : vec2(1058.000000, 305.391235) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.30692
Velocity : vec2(0.000000, 341.015625) Position : vec2(1058.000000, 306.714935) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.3237
Velocity : vec2(0.000000, 345.312500) Position : vec2(1058.000000, 308.055420) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.34048
Velocity : vec2(0.000000, 349.609375) Position : vec2(1058.000000, 309.412689) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.35727
Velocity : vec2(0.000000, 353.906250) Position : vec2(1058.000000, 310.786743) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.37405
Velocity : vec2(0.000000, 358.203125) Position : vec2(1058.000000, 312.177582) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.39084
Velocity : vec2(0.000000, 362.500000) Position : vec2(1058.000000, 313.585205) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.40762
Velocity : vec2(0.000000, 366.796875) Position : vec2(1058.000000, 315.009613) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.42441
Velocity : vec2(0.000000, 371.093750) Position : vec2(1058.000000, 316.450806) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.44119
Velocity : vec2(0.000000, 375.390625) Position : vec2(1058.000000, 317.908783) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.45798
Velocity : vec2(0.000000, 379.687500) Position : vec2(1058.000000, 319.383545) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.47476
Velocity : vec2(0.000000, 383.984375) Position : vec2(1058.000000, 320.875092) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.49155
Velocity : vec2(0.000000, 388.281250) Position : vec2(1058.000000, 322.383423) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.50833
Velocity : vec2(0.000000, 392.578125) Position : vec2(1058.000000, 323.908539) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.52512
Velocity : vec2(0.000000, 396.875000) Position : vec2(1058.000000, 325.450439) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.5419
Velocity : vec2(0.000000, 401.171875) Position : vec2(1058.000000, 327.009125) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.55869
Velocity : vec2(0.000000, 405.468750) Position : vec2(1058.000000, 328.584595) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.57547
Velocity : vec2(0.000000, 409.765625) Position : vec2(1058.000000, 330.176849) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.59225
Velocity : vec2(0.000000, 414.062500) Position : vec2(1058.000000, 331.785889) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.60904
Velocity : vec2(0.000000, 418.359375) Position : vec2(1058.000000, 333.411713) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.62582
Velocity : vec2(0.000000, 422.656250) Position : vec2(1058.000000, 335.054321) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.64261
Velocity : vec2(0.000000, 426.953125) Position : vec2(1058.000000, 336.713715) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.65939
Velocity : vec2(0.000000, 431.250000) Position : vec2(1058.000000, 338.389893) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.67618
Velocity : vec2(0.000000, 435.546875) Position : vec2(1058.000000, 340.082855) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.69296
Velocity : vec2(0.000000, 439.843750) Position : vec2(1058.000000, 341.792603) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.70975
Velocity : vec2(0.000000, 444.140625) Position : vec2(1058.000000, 343.519135) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.72653
Velocity : vec2(0.000000, 448.437500) Position : vec2(1058.000000, 345.262451) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.74332
Velocity : vec2(0.000000, 452.734375) Position : vec2(1058.000000, 347.022552) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.7601
Velocity : vec2(0.000000, 457.031250) Position : vec2(1058.000000, 348.799438) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.77689
Velocity : vec2(0.000000, 461.328125) Position : vec2(1058.000000, 350.593109) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.79367
Velocity : vec2(0.000000, 465.625000) Position : vec2(1058.000000, 352.403564) Acceleration : vec2(0.000000, 1100.000000) Delta vel : 4.29688 Delta pos : 1.81046
Velocity : vec2(0.000000, 462.109375) Position : vec2(1058.000000, 354.215546) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.81198
Velocity : vec2(0.000000, 458.593750) Position : vec2(1058.000000, 356.013794) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.79825
Velocity : vec2(0.000000, 455.078125) Position : vec2(1058.000000, 357.798309) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.78452
Velocity : vec2(0.000000, 451.562500) Position : vec2(1058.000000, 359.569092) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.77078
Velocity : vec2(0.000000, 448.046875) Position : vec2(1058.000000, 361.326141) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.75705
Velocity : vec2(0.000000, 444.531250) Position : vec2(1058.000000, 363.069458) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.74332
Velocity : vec2(0.000000, 441.015625) Position : vec2(1058.000000, 364.799042) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.72958
Velocity : vec2(0.000000, 437.500000) Position : vec2(1058.000000, 366.514893) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.71585
Velocity : vec2(0.000000, 433.984375) Position : vec2(1058.000000, 368.217010) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.70212
Velocity : vec2(0.000000, 430.468750) Position : vec2(1058.000000, 369.905396) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.68839
Velocity : vec2(0.000000, 426.953125) Position : vec2(1058.000000, 371.580048) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.67465
Velocity : vec2(0.000000, 423.437500) Position : vec2(1058.000000, 373.240967) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.66092
Velocity : vec2(0.000000, 419.921875) Position : vec2(1058.000000, 374.888153) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.64719
Velocity : vec2(0.000000, 416.406250) Position : vec2(1058.000000, 376.521606) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.63345
Velocity : vec2(0.000000, 412.890625) Position : vec2(1058.000000, 378.141327) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.61972
Velocity : vec2(0.000000, 409.375000) Position : vec2(1058.000000, 379.747314) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.60599
Velocity : vec2(0.000000, 405.859375) Position : vec2(1058.000000, 381.339569) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.59225
Velocity : vec2(0.000000, 402.343750) Position : vec2(1058.000000, 382.918091) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.57852
Velocity : vec2(0.000000, 398.828125) Position : vec2(1058.000000, 384.482880) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.56479
Velocity : vec2(0.000000, 395.312500) Position : vec2(1058.000000, 386.033936) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.55106
Velocity : vec2(0.000000, 391.796875) Position : vec2(1058.000000, 387.571259) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.53732
Velocity : vec2(0.000000, 388.281250) Position : vec2(1058.000000, 389.094849) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.52359
Velocity : vec2(0.000000, 384.765625) Position : vec2(1058.000000, 390.604706) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.50986
Velocity : vec2(0.000000, 381.250000) Position : vec2(1058.000000, 392.100830) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.49612
Velocity : vec2(0.000000, 377.734375) Position : vec2(1058.000000, 393.583221) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.48239
Velocity : vec2(0.000000, 374.218750) Position : vec2(1058.000000, 395.051880) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.46866
Velocity : vec2(0.000000, 370.703125) Position : vec2(1058.000000, 396.506805) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.45493
Velocity : vec2(0.000000, 367.187500) Position : vec2(1058.000000, 397.947998) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.44119
Velocity : vec2(0.000000, 363.671875) Position : vec2(1058.000000, 399.375458) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.42746
Velocity : vec2(0.000000, 360.156250) Position : vec2(1058.000000, 400.789185) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.41373
Velocity : vec2(0.000000, 356.640625) Position : vec2(1058.000000, 402.189178) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.39999
Velocity : vec2(0.000000, 353.125000) Position : vec2(1058.000000, 403.575439) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.38626
Velocity : vec2(0.000000, 349.609375) Position : vec2(1058.000000, 404.947968) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.37253
Velocity : vec2(0.000000, 346.093750) Position : vec2(1058.000000, 406.306763) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.3588
Velocity : vec2(0.000000, 342.578125) Position : vec2(1058.000000, 407.651825) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.34506
Velocity : vec2(0.000000, 339.062500) Position : vec2(1058.000000, 408.983154) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.33133
Velocity : vec2(0.000000, 335.546875) Position : vec2(1058.000000, 410.300751) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.3176
Velocity : vec2(0.000000, 332.031250) Position : vec2(1058.000000, 411.604614) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.30386
Velocity : vec2(0.000000, 328.515625) Position : vec2(1058.000000, 412.894745) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.29013
Velocity : vec2(0.000000, 325.000000) Position : vec2(1058.000000, 414.171143) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.2764
Velocity : vec2(0.000000, 321.484375) Position : vec2(1058.000000, 415.433807) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.26266
Velocity : vec2(0.000000, 317.968750) Position : vec2(1058.000000, 416.682739) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.24893
Velocity : vec2(0.000000, 314.453125) Position : vec2(1058.000000, 417.917938) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.2352
Velocity : vec2(0.000000, 310.937500) Position : vec2(1058.000000, 419.139404) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.22147
Velocity : vec2(0.000000, 307.421875) Position : vec2(1058.000000, 420.347137) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.20773
Velocity : vec2(0.000000, 303.906250) Position : vec2(1058.000000, 421.541138) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.194
Velocity : vec2(0.000000, 300.390625) Position : vec2(1058.000000, 422.721405) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.18027
Velocity : vec2(0.000000, 296.875000) Position : vec2(1058.000000, 423.887939) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.16653
Velocity : vec2(0.000000, 293.359375) Position : vec2(1058.000000, 425.040741) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.1528
Velocity : vec2(0.000000, 289.843750) Position : vec2(1058.000000, 426.179810) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.13907
Velocity : vec2(0.000000, 286.328125) Position : vec2(1058.000000, 427.305145) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.12534
Velocity : vec2(0.000000, 282.812500) Position : vec2(1058.000000, 428.416748) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.1116
Velocity : vec2(0.000000, 279.296875) Position : vec2(1058.000000, 429.514618) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.09787
Velocity : vec2(0.000000, 275.781250) Position : vec2(1058.000000, 430.598755) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.08414
Velocity : vec2(0.000000, 272.265625) Position : vec2(1058.000000, 431.669159) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.0704
Velocity : vec2(0.000000, 268.750000) Position : vec2(1058.000000, 432.725830) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.05667
Velocity : vec2(0.000000, 265.234375) Position : vec2(1058.000000, 433.768768) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.04294
Velocity : vec2(0.000000, 261.718750) Position : vec2(1058.000000, 434.797974) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.02921
Velocity : vec2(0.000000, 258.203125) Position : vec2(1058.000000, 435.813446) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.01547
Velocity : vec2(0.000000, 254.687500) Position : vec2(1058.000000, 436.815186) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 1.00174
Velocity : vec2(0.000000, 251.171875) Position : vec2(1058.000000, 437.803192) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.988007
Velocity : vec2(0.000000, 247.656250) Position : vec2(1058.000000, 438.777466) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.974274
Velocity : vec2(0.000000, 244.140625) Position : vec2(1058.000000, 439.738007) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.960541
Velocity : vec2(0.000000, 240.625000) Position : vec2(1058.000000, 440.684814) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.946808
Velocity : vec2(0.000000, 237.109375) Position : vec2(1058.000000, 441.617889) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.933075
Velocity : vec2(0.000000, 233.593750) Position : vec2(1058.000000, 442.537231) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.919342
Velocity : vec2(0.000000, 230.078125) Position : vec2(1058.000000, 443.442841) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.905609
Velocity : vec2(0.000000, 226.562500) Position : vec2(1058.000000, 444.334717) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.891876
Velocity : vec2(0.000000, 223.046875) Position : vec2(1058.000000, 445.212860) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.878143
Velocity : vec2(0.000000, 219.531250) Position : vec2(1058.000000, 446.077271) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.86441
Velocity : vec2(0.000000, 216.015625) Position : vec2(1058.000000, 446.927948) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.850677
Velocity : vec2(0.000000, 212.500000) Position : vec2(1058.000000, 447.764893) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.836945
Velocity : vec2(0.000000, 208.984375) Position : vec2(1058.000000, 448.588104) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.823212
Velocity : vec2(0.000000, 205.468750) Position : vec2(1058.000000, 449.397583) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.809479
Velocity : vec2(0.000000, 201.953125) Position : vec2(1058.000000, 450.193329) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.795746
Velocity : vec2(0.000000, 198.437500) Position : vec2(1058.000000, 450.975342) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.782013
Velocity : vec2(0.000000, 194.921875) Position : vec2(1058.000000, 451.743622) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.76828
Velocity : vec2(0.000000, 191.406250) Position : vec2(1058.000000, 452.498169) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.754547
Velocity : vec2(0.000000, 187.890625) Position : vec2(1058.000000, 453.238983) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.740814
Velocity : vec2(0.000000, 184.375000) Position : vec2(1058.000000, 453.966064) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.727081
Velocity : vec2(0.000000, 180.859375) Position : vec2(1058.000000, 454.679413) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.713348
Velocity : vec2(0.000000, 177.343750) Position : vec2(1058.000000, 455.379028) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.699615
Velocity : vec2(0.000000, 173.828125) Position : vec2(1058.000000, 456.064911) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.685883
Velocity : vec2(0.000000, 170.312500) Position : vec2(1058.000000, 456.737061) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.67215
Velocity : vec2(0.000000, 166.796875) Position : vec2(1058.000000, 457.395477) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.658417
Velocity : vec2(0.000000, 163.281250) Position : vec2(1058.000000, 458.040161) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.644684
Velocity : vec2(0.000000, 159.765625) Position : vec2(1058.000000, 458.671112) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.630951
Velocity : vec2(0.000000, 156.250000) Position : vec2(1058.000000, 459.288330) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.617218
Velocity : vec2(0.000000, 152.734375) Position : vec2(1058.000000, 459.891815) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.603485
Velocity : vec2(0.000000, 149.218750) Position : vec2(1058.000000, 460.481567) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.589752
Velocity : vec2(0.000000, 145.703125) Position : vec2(1058.000000, 461.057587) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.576019
Velocity : vec2(0.000000, 142.187500) Position : vec2(1058.000000, 461.619873) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.562286
Velocity : vec2(0.000000, 138.671875) Position : vec2(1058.000000, 462.168427) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.548553
Velocity : vec2(0.000000, 135.156250) Position : vec2(1058.000000, 462.703247) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.534821
Velocity : vec2(0.000000, 131.640625) Position : vec2(1058.000000, 463.224335) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.521088
Velocity : vec2(0.000000, 128.125000) Position : vec2(1058.000000, 463.731689) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.507355
Velocity : vec2(0.000000, 124.609375) Position : vec2(1058.000000, 464.225311) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.493622
Velocity : vec2(0.000000, 121.093750) Position : vec2(1058.000000, 464.705200) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.479889
Velocity : vec2(0.000000, 117.578125) Position : vec2(1058.000000, 465.171356) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.466156
Velocity : vec2(0.000000, 114.062500) Position : vec2(1058.000000, 465.623779) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.452423
Velocity : vec2(0.000000, 110.546875) Position : vec2(1058.000000, 466.062469) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.43869
Velocity : vec2(0.000000, 107.031250) Position : vec2(1058.000000, 466.487427) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.424957
Velocity : vec2(0.000000, 103.515625) Position : vec2(1058.000000, 466.898651) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.411224
Velocity : vec2(0.000000, 100.000000) Position : vec2(1058.000000, 467.296143) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.397491
Velocity : vec2(0.000000, 96.484375) Position : vec2(1058.000000, 467.679901) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.383759
Velocity : vec2(0.000000, 92.968750) Position : vec2(1058.000000, 468.049927) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.370026
Velocity : vec2(0.000000, 89.453125) Position : vec2(1058.000000, 468.406219) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.356293
Velocity : vec2(0.000000, 85.937500) Position : vec2(1058.000000, 468.748779) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.34256
Velocity : vec2(0.000000, 82.421875) Position : vec2(1058.000000, 469.077606) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.328827
Velocity : vec2(0.000000, 78.906250) Position : vec2(1058.000000, 469.392700) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.315094
Velocity : vec2(0.000000, 75.390625) Position : vec2(1058.000000, 469.694061) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.301361
Velocity : vec2(0.000000, 71.875000) Position : vec2(1058.000000, 469.981689) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.287628
Velocity : vec2(0.000000, 68.359375) Position : vec2(1058.000000, 470.255585) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.273895
Velocity : vec2(0.000000, 64.843750) Position : vec2(1058.000000, 470.515747) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.260162
Velocity : vec2(0.000000, 61.328125) Position : vec2(1058.000000, 470.762177) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.246429
Velocity : vec2(0.000000, 57.812500) Position : vec2(1058.000000, 470.994873) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.232697
Velocity : vec2(0.000000, 54.296875) Position : vec2(1058.000000, 471.213837) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.218964
Velocity : vec2(0.000000, 50.781250) Position : vec2(1058.000000, 471.419067) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.205231
Velocity : vec2(0.000000, 47.265625) Position : vec2(1058.000000, 471.610565) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.191498
Velocity : vec2(0.000000, 43.750000) Position : vec2(1058.000000, 471.788330) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.177765
Velocity : vec2(0.000000, 40.234375) Position : vec2(1058.000000, 471.952362) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.164032
Velocity : vec2(0.000000, 36.718750) Position : vec2(1058.000000, 472.102661) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.150299
Velocity : vec2(0.000000, 33.203125) Position : vec2(1058.000000, 472.239227) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.136566
Velocity : vec2(0.000000, 29.687500) Position : vec2(1058.000000, 472.362061) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.122833
Velocity : vec2(0.000000, 26.171875) Position : vec2(1058.000000, 472.471161) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.1091
Velocity : vec2(0.000000, 22.656250) Position : vec2(1058.000000, 472.566528) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.0953674
Velocity : vec2(0.000000, 19.140625) Position : vec2(1058.000000, 472.648163) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.0816345
Velocity : vec2(0.000000, 15.625000) Position : vec2(1058.000000, 472.716064) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.0679016
Velocity : vec2(0.000000, 12.109375) Position : vec2(1058.000000, 472.770233) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.0541687
Velocity : vec2(0.000000, 8.593750) Position : vec2(1058.000000, 472.810669) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.0404358
Velocity : vec2(0.000000, 5.078125) Position : vec2(1058.000000, 472.837372) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.0267029
Velocity : vec2(0.000000, 1.562500) Position : vec2(1058.000000, 472.850342) Acceleration : vec2(0.000000, -900.000000) Delta vel : -3.51562 Delta pos : 0.01297
*/