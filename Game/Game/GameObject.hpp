#pragma once
#include "Sprite.hpp"
#include <glm/glm.hpp>

class GameObject {
public:
	GameObject();
	
	virtual ~GameObject();

	virtual void Update();

	Sprite mSprite;

	glm::vec2 mPosition{ 0.0f, 0.0f };

	glm::vec2 mFlyVelocity{ 0.0f, 0.0f };

	glm::vec2 mFlyDirectionNormalized{ 0.0f, 0.0f };

	glm::mat4 mModelMatrix{ glm::mat4(1.0f) }; 

	glm::vec2 mTriggerAABBPos{ 0.0f, 0.0f };
	glm::vec2 mTriggerAABBSize{ 0.0f, 0.0f };

	float mFlyAngleOrientation{ 0.0f };

	float mFlyAngleTarget{ -1.0f };

	float mFlyAngle{ 0.0f };

	bool mIsDestroyed{ false };

	bool mIsVisible{ false }; 

	bool mIsSucked{ false };

	bool mIsDeathTrigger{ false };

	bool mConsumedByBlackHole{ false };

	bool mIsCollidable{ false };
};