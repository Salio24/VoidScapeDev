#include "EscapePortal.hpp"

EscapePortal::EscapePortal() {
	suctionVelocity = 100.0f;
	mSprite.mVertexData.Position = glm::vec2(17840.0f, 475.0f);
}

EscapePortal::~EscapePortal() {

}

void EscapePortal::Update(Animation& portalAnim, const float& deltaTime, Actor& actor, Mix_Chunk* portalSoundEscape, Mix_Chunk* portalSoundIdle, const float globalSFXVolumeModifier) {
	if (animationOneShot) {
		epicenterAABBPos = glm::vec2(mSprite.mVertexData.Position.x + mSprite.mVertexData.Size.x / 2.0f, mSprite.mVertexData.Position.y + mSprite.mVertexData.Size.y / 4);
		epicenterAABBSize = glm::vec2(mSprite.mVertexData.Size.x / 4, mSprite.mVertexData.Size.y / 2);

		AABBSize = glm::vec2(250.0f, 250.0f);
		AABBPos = glm::vec2(epicenterAABBPos.x + epicenterAABBSize.x / 2.0f - AABBSize.x / 2.0f, epicenterAABBPos.y + epicenterAABBSize.y / 2.0f - AABBSize.y / 2.0f);

		mSprite.mVertexData.TexturePosition = portalAnim.TexturePosition;
		portalAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
		portalAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
		animationOneShot = false;
	}

	if (RectVsRect(AABBPos, AABBSize, actor.mPosition, actor.mSprite.mVertexData.Size) && actor.mIsSuckedPortal == false && actor.mEscaped == false) {
		actor.mIsCollidable = false;
		actor.mIsSuckedPortal = true;
		actor.mFlyDirectionNormalized = glm::normalize(epicenterAABBPos + (epicenterAABBSize / 3.0f) - actor.mPosition);
		actor.mVelocity = actor.mFlyDirectionNormalized * suctionVelocity;
	}
	if (actor.mIsSuckedPortal == true) {
		if (actor.mPosition.x >= epicenterAABBPos.x + epicenterAABBSize.x / 2) {
			actor.mEscaped = true;
			actor.mIsVisible = false;
			actor.mIsSuckedPortal = false;
			Mix_PlayChannel(9, portalSoundEscape, 0);
			actor.mVelocity = glm::vec2(0.0f, 0.0f);
		}

		actor.mVelocity += actor.mVelocity * 6.16f * deltaTime;

		if (actor.mFlyAngleTargetPortal == -1.0f) {
			float dot = glm::dot(actor.mFlyDirectionNormalized, glm::vec2(1.0f, 0.0f));
			actor.mFlyAngleTargetPortal = glm::acos(dot);
		}
		glm::vec2 center = glm::vec2(actor.mSprite.mVertexData.Position.x + actor.mSprite.mVertexData.Size.x / 2, actor.mSprite.mVertexData.Position.y + actor.mSprite.mVertexData.Size.y / 2);

		actor.mModelMatrix = glm::translate(actor.mModelMatrix, glm::vec3(center, 0.0f));

		float crossProduct = glm::cross(actor.mFlyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

		if (crossProduct > 0) {
			actor.mModelMatrix = glm::rotate(actor.mModelMatrix, actor.mFlyAnglePortal, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (crossProduct < 0) {
			actor.mModelMatrix = glm::rotate(actor.mModelMatrix, -actor.mFlyAnglePortal, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		actor.mModelMatrix = glm::translate(actor.mModelMatrix, glm::vec3(-center, 0.0f));

		if (actor.mFlyAnglePortal < actor.mFlyAngleTargetPortal) {
			actor.mFlyAnglePortal += 0.1f * deltaTime;
		}
	}

	float distance = glm::distance2(mSprite.mVertexData.Position + mSprite.mVertexData.Size / 2.0f, actor.mPosition + actor.mSprite.mVertexData.Size / 2.0f);

	if (distance < idleRadius) {
		idleVolume = 128.0f - (distance / idleRadius * 128.0f);
		Mix_Volume(12, idleVolume * globalSFXVolumeModifier);
		if (!Mix_Playing(12)) {
			Mix_PlayChannel(12, portalSoundIdle, 0);
		}
	}
	else {
		idleVolume = 0;
		Mix_Volume(12, idleVolume * globalSFXVolumeModifier);
	}

	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - portalAnim.AnimationTimer).count() > portalAnim.AnimationTime + deltaTime * 1000) {
		portalAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
		portalAnim.index = 0;
	}
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - portalAnim.SingleFrameTimer).count() > portalAnim.SingleFrameTime + deltaTime * 1000) {
		portalAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
		portalAnim.index++;
	}
	if (portalAnim.index > portalAnim.AnimationTextureIndexes.size() - 1) {
		portalAnim.index = 0;
	}
	mSprite.mVertexData.TextureIndex = portalAnim.AnimationTextureIndexes[portalAnim.index];
}
