#include "Actor.hpp"

Actor::Actor() {
	mDoubleJumpOrb.Color = glm::vec4(1.0f);
	mDoubleJumpOrb.Size = glm::vec2(40.0f, 40.0f);
	mDoubleJumpOrb.TexturePosition = glm::vec2(0.0f, 0.0f);
}

Actor::~Actor() = default;

void Actor::Transform() {
	mModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(relativePosition, 0.0f));
}

void Actor::FixedTickrateUpdate(double timeStep, const std::vector<GameObject>* blocks, bool activeKeys[static_cast<int>(ActiveKeys::DUCK)]) {
	mPhysicsComponent.FixedTickrateUpdate(timeStep, blocks, activeKeys, mSprite.mVertexData.Size);
}

void Actor::Update(double accumulator, double timeStep) {
	std::pair<glm::vec2, glm::vec2> interpolated = mPhysicsComponent.Update(accumulator, timeStep);

	mPosition = interpolated.first;
	mVelocity = interpolated.second;

	Move();
	Transform();
}

void Actor::Move() {
	relativePosition = mPosition - mSprite.mVertexData.Position;
}

void Actor::Reset(glm::vec2& animSize, const glm::vec2& startingPosition) {
	mPhysicsComponent.SetPosition(startingPosition);


	mSprite.mVertexData.Position = startingPosition;
	mVelocity = glm::vec2(0.0f, 0.0f);
	mDead = false;
	mConsumedByBlackHole = false;
	mFellDown = false;
	mDiedFromTrigger = false;
	mIsSucked = false;
	mIsSuckedPortal = false;
	mFlyAngle = 0.0f;
	mFlyAnglePortal = 0.0f;
	mFlyAngleTarget = -1.0f;
	mFlyAngleTargetPortal = -1.0f;
	mDeadSoundOneShot = true;
	mIsVisible = true;
	mIsCollidable = true;
	mEscaped = false;
	mPosition = mSprite.mVertexData.Position;
	mSprite.mVertexData.Size = animSize * mSizeMultiplier;
}