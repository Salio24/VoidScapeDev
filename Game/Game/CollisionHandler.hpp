#pragma once
#include <limits>
#include <algorithm>
#include <cmath>
#include "HelperStructs.hpp"
#include <glm/glm.hpp>
#include "GameObject.hpp"

class Actor;

bool PointVsRect(const glm::vec2& point, const glm::vec2& boxSize, const glm::vec2& boxPos);

bool RectVsRect(const glm::vec2 rect1Pos, const glm::vec2 rect1Size, const glm::vec2 rect2Pos, const glm::vec2 rect2Size);

bool RayVsRect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const Box* target, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear);

bool DynamicRectVsRect(const Box& dynamicBox, const float deltaTime, const Box& staticBox, const glm::vec2& dynamicBoxVelocity, glm::vec2& contactPoint, glm::vec2& contactNormal, float& contactTime, glm::vec2& position);

bool ResolveDynamicRectVsRect(Box& dynamicBox, const float deltaTime, const Box& staticBox, glm::vec2& dynamicBoxVelocity, Actor& actor, glm::vec2& averagedNormal, bool& NormalGroundCheck, bool& isWallMountableL, bool& isWallMountableR, float& Ynormal);

void CollisionUpdate(const std::vector<GameObject>* blocks, Actor& actor, bool& LeftWallHug, bool& RightWallHug, const float& deltaTime, bool& isGrounded, bool& isWallMountableL, bool& isWallMountableR);