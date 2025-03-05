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

	accF = glm::vec2(0.0f, 0.0f);

	//accF += glm::vec2(0.0f, -10.0f);

	if (testButton1) {
		accF += glm::vec2(0.0f, 100.0f);
	}
	if (testButton3) {
		accF += glm::vec2(-80.0f, 0.0f);
	}
	if (testButton4) {
		accF += glm::vec2(80.0f, 0.0f);
	}
	if (testButton2) {
		accF += glm::vec2(0.0f, -100.0f);
	}

	return accF;
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

void PhysicsComponent::FixedTickrateUpdate(double deltaTime, const std::vector<GameObject>* blocks) {

	// add 2 distinct structs, static box and dynamic box

	accumulator += deltaTime;

	while (accumulator >= timeStep) {
		previous = current;
		RK4_IntegrateVec2(current, time, timeStep);
		CUpdate(blocks, previous.position, current.position, current.velocity, timeStep);

		time += timeStep;
		accumulator -= timeStep;
	}

	const double alpha = accumulator / timeStep;

	mInterpolatedState.position = MultiplyVec2AndDouble(current.position, alpha) + MultiplyVec2AndDouble(previous.position, (1.0f - alpha));
	mInterpolatedState.velocity = MultiplyVec2AndDouble(current.velocity, alpha) + MultiplyVec2AndDouble(previous.velocity, (1.0f - alpha));

}

bool PhysicsComponent::RayVsRectT(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const Box* target, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear)
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

bool PhysicsComponent::DynamicRectVsRectT(const glm::vec2& size, const float deltaTime, const Box& staticBox, const glm::vec2& dynamicBoxVelocity,
	glm::vec2& contactPoint, glm::vec2& contactNormal, float& contactTime, glm::vec2& position)
{
	if (dynamicBoxVelocity.x == 0 && dynamicBoxVelocity.y == 0)
		return false;

	Box expanded_target;
	expanded_target.Position = staticBox.Position - (size / 2.0f);
	expanded_target.Size = staticBox.Size + size;

	if (RayVsRectT(position + (size / 2.0f), dynamicBoxVelocity * deltaTime, &expanded_target, contactPoint, contactNormal, contactTime)) {
		return (contactTime >= 0.0f && contactTime < 1.0f);
	}
	else {
		return false;
	}
}

bool PhysicsComponent::ResolveDynamicRectVsRectT(const glm::vec2& size, const float deltaTime, const Box& staticBox, glm::vec2 dynamicBoxVelocity, glm::vec2& pos, glm::vec2& impulse)
{

	glm::vec2 contactPoint, contactNormal;
	float contactTime = 0.0f;
	if (DynamicRectVsRectT(size, deltaTime, staticBox, dynamicBoxVelocity, contactPoint, contactNormal, contactTime, pos))
	{
		float vRel = glm::dot(dynamicBoxVelocity, contactNormal);

		if (vRel < 0)
		{
			float restitution = 0.0f;

			float j = -(1.0f + restitution) * vRel;

			impulse = j * contactNormal;
		}

		//impulse = contactNormal * glm::vec2(std::abs(dynamicBoxVelocity.x), std::abs(dynamicBoxVelocity.y)) * (1 - contactTime);


		return true;
	}
	return false;
}



void PhysicsComponent::CUpdate(const std::vector<GameObject>* blocks, glm::vec2 previousPos, glm::vec2& currentPos, glm::vec2& velocity, double timeStep) {

	ResolvePenetration(blocks, currentPos, velocity, glm::vec2(40.0f));


	glm::vec2 contactPoint, contactNormal;
	float contactTime = 0.0f;
	std::vector<std::pair<int, float>> collidedBlocks;
	glm::vec2 dynamicBoxVelocity = (currentPos - previousPos) / (float)timeStep; // Average velocity for collision detection

	// Detect collisions along the path from previousPos to currentPos
	for (int i = 0; i < blocks->size(); i++) {
		if (!blocks->at(i).mIsDeathTrigger && blocks->at(i).mIsCollidable) {
			if (DynamicRectVsRectT(glm::vec2(40.0f), timeStep, blocks->at(i).mSprite.mVertexData, dynamicBoxVelocity, contactPoint, contactNormal, contactTime, previousPos)) {
				collidedBlocks.push_back({ i, contactTime });
				collBlocks.push_back(blocks->at(i).mSprite.mVertexData.Position);

			}
		}
	}

	std::sort(collidedBlocks.begin(), collidedBlocks.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
		return a.second < b.second;
		});

	glm::vec2 totalImpulse = glm::vec2(0.0f);
	std::set<float> resolvedNormalsX; // Tracks which normals have been handled

	std::set<float> resolvedNormalsY; // Tracks which normals have been handled

	float earliestTime = 1.0f;

	// Resolve collisions
	for (auto j : collidedBlocks) {
		glm::vec2 impulse = glm::vec2(0.0f);
	
		glm::vec2 cp, cn;
		float ct = 0;
	
		if (DynamicRectVsRectT(glm::vec2(40.0f), timeStep, blocks->at(j.first).mSprite.mVertexData, dynamicBoxVelocity, cp, cn, ct, previousPos)) {
			if (!resolvedNormalsX.contains(cn.x) && !resolvedNormalsY.contains(cn.y)) {
				float vRel = glm::dot(velocity, cn);
				if (cn.x != 0 && cn.y != 1) {
					std::cout << "vRel: " << vRel << ", Velocity: (" << current.velocity.x << ", " << current.velocity.y << "), Normal: (" << cn.x << ", " << cn.y << ")" << std::endl;
				}
				if (vRel < 0) {
					float restitution = 0.0f; // Adjust as needed
					float jMag = -(1.0f + restitution) * vRel;
					impulse = jMag * cn;
					totalImpulse += impulse;

					earliestTime = std::min(earliestTime, contactTime);

					if (cn.x != 0) {
						resolvedNormalsX.insert(cn.x);
					}
					if (cn.y != 0) {
						resolvedNormalsY.insert(cn.y);
					}
				}
			}
		}
		//collBlocks.push_back(blocks->at(j.first).mSprite.mVertexData.Position);
	
		// Compute impulse using current velocity (post-integration)
	}

	// Apply total impulse to velocity

	// see last grok response 

	if (totalImpulse.y > 0 && totalImpulse.x != 0) {
		std::cout << "Total Impulse: " << glm::to_string(totalImpulse) << std::endl;
	}

	if (resolvedNormalsX.size() > 0 || resolvedNormalsY.size()> 0) {
		//currentPos = previousPos + dynamicBoxVelocity * (float)timeStep * earliestTime;
		velocity += totalImpulse;

	}

	//ResolvePenetration(blocks, currentPos, velocity, glm::vec2(40.0f));

}

void PhysicsComponent::ResolvePenetration(const std::vector<GameObject>* blocks, glm::vec2& position, glm::vec2& velocity, glm::vec2 size) {
	for (const auto& block : *blocks) {
		if (!block.mIsDeathTrigger && block.mIsCollidable) {
			// AABB bounds: character
			glm::vec2 minA = position;
			glm::vec2 maxA = position + size;
			// AABB bounds: static block
			glm::vec2 minB = block.mSprite.mVertexData.Position;
			glm::vec2 maxB = minB + block.mSprite.mVertexData.Size;

			// Calculate overlap in each axis
			float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
			float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);

			// Check if there’s an overlap
			if (overlapX > 0 && overlapY > 0) {
				// Resolve along the axis with the smallest penetration
				if (overlapX < overlapY) {
					// Resolve along X-axis
					if (minA.x < minB.x) {
						position.x -= overlapX; // Move left
						if (velocity.x > 0) velocity.x = 0; // Stop rightward motion
					}
					else {
						position.x += overlapX; // Move right
						if (velocity.x < 0) velocity.x = 0; // Stop leftward motion
					}
				}
				else {
					// Resolve along Y-axis
					if (minA.y < minB.y) {
						position.y -= overlapY; // Move down (unlikely, but included for completeness)
						if (velocity.y > 0) velocity.y = 0;
					}
					else {
						position.y += overlapY; // Move up
						if (velocity.y < 0) velocity.y = 0; // Stop downward motion
					}
				}
			}
		}
	}
}

/*
void PhysicsComponent::CUpdate(const std::vector<GameObject>* blocks, glm::vec2 pos, glm::vec2 size, glm::vec2 vel) {
	glm::vec2 contactPoint, contactNormal;

	float contactTime = 0;

	std::vector<std::pair<int, float>> colidedBlocks;

	glm::vec2 A(pos.x - 3 * 40.0f, pos.y - 3 * 40.0f);
	// Top right corner of broad-phase-box
	glm::vec2 B(pos.x + 4 * 40.0f, pos.y + 4 * 40.0f);

	for (int i = 0; i < blocks->size(); i++) {
		if (blocks->at(i).mSprite.mVertexData.Position.x > A.x && blocks->at(i).mSprite.mVertexData.Position.x < B.x && blocks->at(i).mSprite.mVertexData.Position.y > A.y && blocks->at(i).mSprite.mVertexData.Position.y < B.y) {
			if (!blocks->at(i).mIsDeathTrigger && blocks->at(i).mIsCollidable == true) {
				if (DynamicRectVsRectT(size, 1.0f, blocks->at(i).mSprite.mVertexData, vel, contactPoint, contactNormal, contactTime, pos)) {
					colidedBlocks.push_back({ i, contactTime });
				}
			}
		}
	}

	std::sort(colidedBlocks.begin(), colidedBlocks.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
		return a.second < b.second;
		});

	glm::vec2 totalImpulse = glm::vec2(0.0f);

	// Resolve collisions
	for (const auto& j : colidedBlocks) {
		glm::vec2 impulse = glm::vec2(0.0f);
		Box staticBox = blocks->at(j.first).mSprite.mVertexData;

		// Compute impulse using current velocity (post-integration)
		float vRel = glm::dot(vel, contactNormal);
		if (vRel < 0) {
			float restitution = 0.0f; // Adjust as needed
			float jMag = -(1.0f + restitution) * vRel;
			impulse = jMag * contactNormal;
			totalImpulse += impulse;
		}
	}

	// Apply total impulse to velocity
	current.velocity += totalImpulse;

	std::cout << glm::to_string(current.velocity) << std::endl;

}
*/