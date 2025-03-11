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

		time += timeStep;
		accumulator -= timeStep;
	}


	const double alpha = accumulator / timeStep;

	mInterpolatedState.position = MultiplyVec2AndDouble(current.position, alpha) + MultiplyVec2AndDouble(previous.position, (1.0f - alpha));
	mInterpolatedState.velocity = MultiplyVec2AndDouble(current.velocity, alpha) + MultiplyVec2AndDouble(previous.velocity, (1.0f - alpha));

	//std::cout << glm::to_string(current.velocity) << glm::to_string(mInterpolatedState.position) << glm::to_string(normal) << glm::to_string(frictest) << std::endl;


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
			std::cout << "a" << std::endl;
		}


	}

	if (mGrounded && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpBufferTimer).count() < 200) {
		jumpBufferTimer = std::chrono::time_point<std::chrono::steady_clock>{};
		isJumping = true;

		ticks = 0;

		acceleration.y += mPhysicsSettings.JumpStartImpulse;
		mGrounded = false;
		std::cout << "bv" << std::endl;
	}
	if (mGrounded || current.velocity.y < 0.0f) {
		isJumping = false;
	}

}