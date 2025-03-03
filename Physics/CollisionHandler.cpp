#include "CollisionHandler.hpp"
#include "../Actors//Actor.hpp"
#include <iostream> // debug


bool PointVsRect(const glm::vec2& point, const glm::vec2& boxSize, const glm::vec2& boxPos)
{
	return (point.x >= boxPos.x && point.y >= boxPos.y && point.x < boxPos.x + boxSize.x && point.y < boxPos.y + boxSize.y);
}

bool RectVsRect(const glm::vec2 rect1Pos, const glm::vec2 rect1Size, const glm::vec2 rect2Pos, const glm::vec2 rect2Size)
{
	return (rect1Pos.x < rect2Pos.x + rect2Size.x && rect1Pos.x + rect1Size.x > rect2Pos.x && rect1Pos.y < rect2Pos.y + rect2Size.y && rect1Pos.y + rect1Size.y > rect2Pos.y);
}

bool RayVsRect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const Box* target, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear)
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

bool DynamicRectVsRect(const Box& dynamicBox, const float deltaTime, const Box& staticBox, const glm::vec2& dynamicBoxVelocity,
	glm::vec2& contactPoint, glm::vec2& contactNormal, float& contactTime, glm::vec2& position)
{
	if (dynamicBoxVelocity.x == 0 && dynamicBoxVelocity.y == 0)
		return false;

	Box expanded_target;
	expanded_target.Position = staticBox.Position - ( dynamicBox.Size / 2.0f );
	expanded_target.Size = staticBox.Size + dynamicBox.Size;
	
	if (RayVsRect(position + (dynamicBox.Size / 2.0f), dynamicBoxVelocity * deltaTime, &expanded_target, contactPoint, contactNormal, contactTime)) {
		return (contactTime >= 0.0f && contactTime < 1.0f);
	}
	else {
		return false;
	}
}

bool ResolveDynamicRectVsRect(Box& dynamicBox, const float deltaTime, const Box& staticBox, glm::vec2& dynamicBoxVelocity, Actor& actor, glm::vec2& averagedNormal, bool& NormalGroundCheck, bool& isWallMountableL, bool& isWallMountableR, float& Ynormal)
{

	glm::vec2 contactPoint, contactNormal;
	float contactTime = 0.0f;
	if (DynamicRectVsRect(actor.mSprite.mVertexData, deltaTime, staticBox, actor.mVelocity, contactPoint, contactNormal, contactTime, actor.mPosition))
	{
		//if (contactNormal.y > 0) dynamicBox->contact[0] = staticBox; else nullptr;
		//if (contactNormal.x < 0) dynamicBox->contact[1] = staticBox; else nullptr;
		//if (contactNormal.y < 0) dynamicBox->contact[2] = staticBox; else nullptr;
		//if (contactNormal.x > 0) dynamicBox->contact[3] = staticBox; else nullptr;

		if (contactNormal.x >= 1 && contactNormal.x != 0 && actor.mVelocity.y != 0) {
			isWallMountableL = true;
		}
		else {
			isWallMountableL = false;
		}
		if (contactNormal.x <= -1 && contactNormal.x != 0 && actor.mVelocity.y != 0) {
			isWallMountableR = true;
		}
		else {
			isWallMountableR = false;
		}

		averagedNormal = glm::mix(averagedNormal, contactNormal, 0.5f);

		if (averagedNormal.y > 0.25f) {
			NormalGroundCheck = true;
		}
		else {
			NormalGroundCheck = false;
		}

		actor.mVelocity += contactNormal * glm::vec2(std::abs(actor.mVelocity.x), std::abs(actor.mVelocity.y)) * (1 - contactTime);
		if (contactNormal.x < 0) {
		
		}

		Ynormal = contactNormal.y;

		return true;
	}
	return false;
}

void CollisionUpdate(const std::vector<GameObject>* blocks, Actor& actor, bool& LeftWallHug, bool& RightWallHug, const float& deltaTime, bool& isGrounded, bool& isWallMountableL, bool& isWallMountableR) {

	static glm::vec2 averagedNormal(0.0f, 0.0f);
	static bool NormalGroundCheck = false;
	static bool BottomWallHug = false;
	
	glm::vec2 contactPoint, contactNormal;

	glm::vec2 contactPoint2, contactNormal2;

	float contactTime = 0;

	std::vector<std::pair<int, float>> colidedBlocks;

	// Bottom left corner of broad-phase-box
	glm::vec2 A(actor.mPosition.x - 3 * actor.mSprite.mVertexData.Size.x, actor.mPosition.y - 3 * actor.mSprite.mVertexData.Size.y);
	// Top right corner of broad-phase-box
	glm::vec2 B(actor.mPosition.x + 4 * actor.mSprite.mVertexData.Size.x, actor.mPosition.y + 4 * actor.mSprite.mVertexData.Size.y);
	
	// use normals for those as well
	LeftWallHug   = false;
	RightWallHug  = false;
	BottomWallHug = false;
	for (int i = 0; i < blocks->size(); i++) {
		if (blocks->at(i).mSprite.mVertexData.Position.x > A.x && blocks->at(i).mSprite.mVertexData.Position.x < B.x && blocks->at(i).mSprite.mVertexData.Position.y > A.y && blocks->at(i).mSprite.mVertexData.Position.y < B.y && actor.mIsCollidable == true) {
			if (!blocks->at(i).mIsDeathTrigger && blocks->at(i).mIsCollidable == true) {
				if (DynamicRectVsRect(actor.mSprite.mVertexData, deltaTime, blocks->at(i).mSprite.mVertexData, actor.mVelocity, contactPoint, contactNormal, contactTime, actor.mPosition)) {
					colidedBlocks.push_back({ i, contactTime });
				}
				if (blocks->at(i).mSprite.mVertexData.Position.x == actor.mPosition.x + actor.mSprite.mVertexData.Size.x && actor.mPosition.y < blocks->at(i).mSprite.mVertexData.Position.y + blocks->at(i).mSprite.mVertexData.Size.y && actor.mPosition.y + actor.mSprite.mVertexData.Size.y > blocks->at(i).mSprite.mVertexData.Position.y && !isGrounded) {
					RightWallHug = RectVsRect(glm::vec2(actor.mPosition.x + actor.mSprite.mVertexData.Size.x / 2, actor.mPosition.y), actor.mSprite.mVertexData.Size, blocks->at(i).mSprite.mVertexData.Position, blocks->at(i).mSprite.mVertexData.Size);
				}
				if (blocks->at(i).mSprite.mVertexData.Position.x + blocks->at(i).mSprite.mVertexData.Size.x == actor.mPosition.x && actor.mPosition.y < blocks->at(i).mSprite.mVertexData.Position.y + blocks->at(i).mSprite.mVertexData.Size.y && actor.mPosition.y + actor.mSprite.mVertexData.Size.y > blocks->at(i).mSprite.mVertexData.Position.y && !isGrounded) {
					LeftWallHug = RectVsRect(glm::vec2(actor.mPosition.x - actor.mSprite.mVertexData.Size.x / 2, actor.mPosition.y), actor.mSprite.mVertexData.Size, blocks->at(i).mSprite.mVertexData.Position, blocks->at(i).mSprite.mVertexData.Size);
				} 
				if (blocks->at(i).mSprite.mVertexData.Position.y + blocks->at(i).mSprite.mVertexData.Size.y == actor.mPosition.y && actor.mPosition.x < blocks->at(i).mSprite.mVertexData.Position.x + blocks->at(i).mSprite.mVertexData.Size.x && actor.mPosition.x + actor.mSprite.mVertexData.Size.x > blocks->at(i).mSprite.mVertexData.Position.x) {
					BottomWallHug = RectVsRect(glm::vec2(actor.mPosition.x, actor.mPosition.y - actor.mSprite.mVertexData.Size.y / 2), actor.mSprite.mVertexData.Size, blocks->at(i).mSprite.mVertexData.Position, blocks->at(i).mSprite.mVertexData.Size);
				}
			}
			else if (blocks->at(i).mIsDeathTrigger) {
				Box AABB;
				AABB.Position = blocks->at(i).mTriggerAABBPos;
				AABB.Size = blocks->at(i).mTriggerAABBSize;
				if (DynamicRectVsRect(actor.mSprite.mVertexData, deltaTime, AABB, actor.mVelocity, contactPoint2, contactNormal2, contactTime, actor.mPosition)) {
					if (blocks->at(i).mIsCollidable) {
						colidedBlocks.push_back({ i, contactTime });
					}
					actor.mDead = true;
					actor.mDiedFromTrigger = true;
				}
				if (blocks->at(i).mSprite.mVertexData.Position.y + blocks->at(i).mSprite.mVertexData.Size.y == actor.mPosition.y && actor.mPosition.x < blocks->at(i).mSprite.mVertexData.Position.x + blocks->at(i).mSprite.mVertexData.Size.x && actor.mPosition.x + actor.mSprite.mVertexData.Size.x > blocks->at(i).mSprite.mVertexData.Position.x) {
					if (blocks->at(i).mIsCollidable) {
						BottomWallHug = RectVsRect(glm::vec2(actor.mPosition.x, actor.mPosition.y - actor.mSprite.mVertexData.Size.y / 2), actor.mSprite.mVertexData.Size, blocks->at(i).mSprite.mVertexData.Position, blocks->at(i).mSprite.mVertexData.Size);
					}
				}
			}
		}
	}

	std::sort(colidedBlocks.begin(), colidedBlocks.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
		return a.second < b.second;
		});


	float yNorm = 2;

	for (auto j : colidedBlocks) {
		ResolveDynamicRectVsRect(actor.mSprite.mVertexData, deltaTime, blocks->at(j.first).mSprite.mVertexData, actor.mVelocity, actor, averagedNormal, NormalGroundCheck, isWallMountableL, isWallMountableR, yNorm);
	}

	if (yNorm == 1) {
		isGrounded = true;
	}
	else {
		isGrounded = false;
	}


	actor.mPosition += actor.mVelocity * deltaTime;


	actor.mPosition = glm::vec2(float(std::round(actor.mPosition.x * 1000)) / 1000.0f, float(std::round(actor.mPosition.y * 1000)) / 1000.0f);
}