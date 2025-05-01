
#include "VSMath.hpp"

namespace {
	glm::vec2 RK4_AccelerationVec2(const PhysicsState& state, double time, glm::vec2 acceleratinon) {
		return acceleratinon;
	}

	Derivative RK4_EvaluateVec2(PhysicsState& initial, glm::vec2 acceleratinon, double time, double timeStep, const Derivative& d) {
		PhysicsState state;

		state.position = initial.position + VSMath::MultiplyVec2AndDouble(d.dpos, timeStep);
		state.velocity = initial.velocity + VSMath::MultiplyVec2AndDouble(d.dvel, timeStep);

		Derivative output;
		output.dpos = state.velocity;
		output.dvel = RK4_AccelerationVec2(state, time + timeStep, acceleratinon);

		return output;
	}
}

namespace VSMath {
	glm::vec2 MultiplyVec2AndDouble(glm::vec2 vec, double d) {
		return glm::vec2(vec.x * d, vec.y * d);
	}

	void RK4_IntegrateVec2(PhysicsState& state, double time, double timeStep, glm::vec2 acceleration) {
		Derivative a, b, c, d;

		a = RK4_EvaluateVec2(state, acceleration, time, 0.0f, Derivative());
		b = RK4_EvaluateVec2(state, acceleration, time, timeStep * 0.5f, a);
		c = RK4_EvaluateVec2(state, acceleration, time, timeStep * 0.5f, b);
		d = RK4_EvaluateVec2(state, acceleration, time, timeStep, c);

		glm::vec2 positionChange = 1.0f / 6.0f * (a.dpos + 2.0f * (b.dpos + c.dpos) + d.dpos);
		glm::vec2 velocityChange = 1.0f / 6.0f * (a.dvel + 2.0f * (b.dvel + c.dvel) + d.dvel);

		state.position = state.position + MultiplyVec2AndDouble(positionChange, timeStep);
		state.velocity = state.velocity + MultiplyVec2AndDouble(velocityChange, timeStep);
	}

	namespace Physics {
		bool PointVsRect(const glm::vec2 point, const glm::vec2 boxSize, const glm::vec2 boxPos) {
			return (point.x >= boxPos.x && point.y >= boxPos.y && point.x < boxPos.x + boxSize.x && point.y < boxPos.y + boxSize.y);
		}

		bool RectVsRect(const glm::vec2 rect1Pos, const glm::vec2 rect1Size, const glm::vec2 rect2Pos, const glm::vec2 rect2Size) {
			return (rect1Pos.x < rect2Pos.x + rect2Size.x && rect1Pos.x + rect1Size.x > rect2Pos.x && rect1Pos.y < rect2Pos.y + rect2Size.y && rect1Pos.y + rect1Size.y > rect2Pos.y);
		}

		bool RayVsRect(const glm::vec2 rayOrigin, const glm::vec2 rayDirection, const glm::vec2 targetPos, const glm::vec2 targetSize, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear) {
			contactNormal = { 0,0 };
			contactPoint = { 0,0 };

			// Cache division
			glm::vec2 invDirection = 1.0f / rayDirection;

			// Calculate intersections with rectangle bounding axes
			glm::vec2 timeNear = (targetPos - rayOrigin) * invDirection;
			glm::vec2 timeFar = (targetPos + targetSize - rayOrigin) * invDirection;

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
		bool ResolvePenetrationOnX(glm::vec2& dynamicPos, glm::vec2& dynamicVel, glm::vec2 dynamicSize, glm::vec2 staticPos, glm::vec2 staticSize, glm::vec2& normal) {
			glm::vec2 minA = dynamicPos;
			glm::vec2 maxA = minA + dynamicSize;
			// AABB bounds: static block
			glm::vec2 minB = staticPos;
			glm::vec2 maxB = minB + staticSize;

			// Calculate overlap in each axis
			float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
			float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);

			if (overlapX > 0 && overlapY > 0) {
				// Resolve along the axis with the smallest penetration
				if (overlapX < overlapY) {
					// Resolve along X-axis
					if (minA.x < minB.x) {
						dynamicPos.x -= overlapX; // Move left
						normal.x = -1.0f;
						if (dynamicVel.x > 0) dynamicVel.x = 0;
					}
					else {
						dynamicPos.x += overlapX; // Move right
						normal.x = 1.0f;
						if (dynamicVel.x < 0) dynamicVel.x = 0;
					}
					return true;
				}
			}
			return false;
		}

		bool ResolvePenetrationOnY(glm::vec2& dynamicPos, glm::vec2& dynamicVel, glm::vec2 dynamicSize, glm::vec2 staticPos, glm::vec2 staticSize, glm::vec2& normal) {
			glm::vec2 minA = dynamicPos;
			glm::vec2 maxA = minA + dynamicSize;
			// AABB bounds: static block
			glm::vec2 minB = staticPos;
			glm::vec2 maxB = minB + staticSize;

			// Calculate overlap in each axis
			float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
			float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);

			if (overlapX > 0 && overlapY > 0) {
				// Resolve along the axis with the smallest penetration
				if (overlapX > overlapY) {
					// Resolve along Y-axis
					if (minA.y < minB.y) {
						dynamicPos.y -= overlapY; // Move down 
						normal.y = -1.0f;
						if (dynamicVel.y > 0) dynamicVel.y = 0;
					}
					else {
						dynamicPos.y += overlapY; // Move up
						normal.y = 1.0f;
						if (dynamicVel.y < 0) dynamicVel.y = 0;
					}
					return true;
				}
			}
			return false;
		}
	}
}