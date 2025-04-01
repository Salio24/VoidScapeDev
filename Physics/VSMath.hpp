#pragma once
#include "../Utilities/HelperStructs.hpp"
#include <glm/glm.hpp>

namespace VSMath {

	// make like vs core, then math, and phyics,
	// check how hazel engine does that
	void RK4_IntegrateVec2(PhysicsState& state, double time, double timeStep, glm::vec2 acceleration);

	glm::vec2 MultiplyVec2AndDouble(glm::vec2 vec, double d);

	namespace Physics {
		bool PointVsRect(const glm::vec2& point, const glm::vec2& boxSize, const glm::vec2& boxPos);

		bool RectVsRect(const glm::vec2 rect1Pos, const glm::vec2 rect1Size, const glm::vec2 rect2Pos, const glm::vec2 rect2Size);

		bool RayVsRect(const glm::vec2 rayOrigin, const glm::vec2 rayDirection, const glm::vec2 targetPos, const glm::vec2 targetSize, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear);

		bool ResolvePenetrationOnX(glm::vec2& dynamicPos, glm::vec2& dynamicVel, glm::vec2 dynamicSize, glm::vec2 staticPos, glm::vec2 staticSize, glm::vec2& normal);

		bool ResolvePenetrationOnY(glm::vec2& dynamicPos, glm::vec2& dynamicVel, glm::vec2 dynamicSize, glm::vec2 staticPos, glm::vec2 staticSize, glm::vec2& normal);
	}
}