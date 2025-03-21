#include "BlackHole.hpp" 

BlackHole::BlackHole() : gen(rd()), randomVelocity(1.0f, 5.0f) {
	mSprite.mVertexData.Size = glm::vec2(400.0f, 400.0f);
	AABBVelocityMultiplier = 360.0f;

	mSprite.mVertexData.Position = (epicenterAABBPos + epicenterAABBSize / 2.0f) - mSprite.mVertexData.Size / 2.0f;
}

BlackHole::~BlackHole() {

}

void BlackHole::Update(std::vector<GameObject>* blocks, Actor& actor, const float& deltaTime, Animation& birthAnim, Animation& loopAnim, Mix_Chunk* bornSound, Mix_Chunk* consumedSound, Mix_Chunk* blackHoleIdle, const float globalSFXVolumeModifier, DeathCause actorDeathCause) {
	if (!birthTimerOneShot && !isBorn) {
		mSprite.mVertexData.TexturePosition = loopAnim.TexturePosition;
		birthAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
		birthAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
		Mix_PlayChannel(8, bornSound, 0);
		birthTimerOneShot = true;
	}

	if (!isBorn) {
		if (!(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - birthAnim.AnimationTimer).count() > birthAnim.AnimationTime + deltaTime * 1000)) {
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - birthAnim.SingleFrameTimer).count() > birthAnim.SingleFrameTime + deltaTime * 1000) {
				birthAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
				birthAnim.index++;
			}
			mSprite.mVertexData.TextureIndex = birthAnim.AnimationTextureIndexes[birthAnim.index];
		}
		else {
			birthAnim.index = 0;
			isBorn = true;
		}
		if (birthAnim.index > birthAnim.AnimationTextureIndexes.size() - 1) {
			birthAnim.index = 0;
		}
	}

	if (isBorn) {
		if (!loopTimerOneShot) {
			loopAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
			loopAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			loopTimerOneShot = true;
		}
		mSprite.mVertexData.TextureIndex = loopAnim.AnimationTextureIndexes[loopAnim.index];
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - loopAnim.AnimationTimer).count() > loopAnim.AnimationTime + deltaTime * 1000) {
			loopAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
			loopAnim.index = 0;
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - loopAnim.SingleFrameTimer).count() > loopAnim.SingleFrameTime + deltaTime * 1000) {
			loopAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			loopAnim.index++;
		}
		if (loopAnim.index > loopAnim.AnimationTextureIndexes.size() - 1) {
			loopAnim.index = 0;
		}
	}


	// comment to disable black hole "spread"
	AABBSize.x += AABBVelocityMultiplier * deltaTime;

	///*
	if (RectVsRect(AABBPos, AABBSize, actor.mPosition, actor.mSprite.mVertexData.Size) && actor.mIsSucked == false && actor.mConsumedByBlackHole == false && actor.mIsSuckedPortal == false && actor.mEscaped == false && !(actor.mDead && actorDeathCause != DeathCause::LAVA)) {
		actor.mIsCollidable = false;
		actor.mIsSucked = true;
		actor.mFlyDirectionNormalized = glm::normalize(epicenterAABBPos - actor.mPosition);
		actor.mVelocity = actor.mFlyDirectionNormalized * (float)randomVelocity(gen);
	}
	if (actor.mIsSucked == true) {
		if (actor.mPosition.x <= epicenterAABBPos.x + epicenterAABBSize.x / 2) {
			actor.mConsumedByBlackHole = true;
			actor.mDead = true;
			actor.mIsVisible = false;
			actor.mIsSucked = false;
			Mix_PlayChannel(11, consumedSound, 0);
			actor.mVelocity = glm::vec2(0.0f, 0.0f);
		}

		actor.mVelocity += actor.mVelocity * 2.16f * deltaTime;
		
		if (actor.mFlyAngleTarget == -1.0f) {
			float dot = glm::dot(actor.mFlyDirectionNormalized, glm::vec2(-1.0f, 0.0f));
			actor.mFlyAngleTarget = glm::acos(dot);
		}
		glm::vec2 center = glm::vec2(actor.mSprite.mVertexData.Position.x + actor.mSprite.mVertexData.Size.x / 2, actor.mSprite.mVertexData.Position.y + actor.mSprite.mVertexData.Size.y / 2);

		actor.mModelMatrix = glm::translate(actor.mModelMatrix, glm::vec3(center, 0.0f));

		float crossProduct = glm::cross(actor.mFlyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

		if (crossProduct > 0) {
			actor.mModelMatrix = glm::rotate(actor.mModelMatrix, -actor.mFlyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (crossProduct < 0) {
			actor.mModelMatrix = glm::rotate(actor.mModelMatrix, actor.mFlyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		actor.mModelMatrix = glm::translate(actor.mModelMatrix, glm::vec3(-center, 0.0f));

		if (actor.mFlyAngle < actor.mFlyAngleTarget) {
			actor.mFlyAngle += 0.1f * deltaTime;
		}
	}
	//*/

	//maybe create a vector of all sucked blocks
	// create a check to cull blocks that are further than the black hole aabb
	for (int i = 0; i < blocks->size(); i++) {
		if (blocks->at(i).mIsSucked == true && blocks->at(i).mConsumedByBlackHole == false) {
			blocks->at(i).mIsCollidable = false;
			blocks->at(i).mSprite.mVertexData.Position = blocks->at(i).mSprite.mVertexData.Position + blocks->at(i).mFlyVelocity * deltaTime;
			if (blocks->at(i).mFlyVelocity.x > -100000.0f) {
			blocks->at(i).mFlyVelocity += blocks->at(i).mFlyVelocity * 2.16f * deltaTime;
			}

			if (blocks->at(i).mSprite.mVertexData.Position.x <= epicenterAABBPos.x + epicenterAABBSize.x / 2) {
				blocks->at(i).mConsumedByBlackHole = true;
				blocks->at(i).mIsVisible = false;
				blocks->at(i).mIsSucked = false;
				blocks->at(i).mFlyVelocity = glm::vec2(0.0f, 0.0f);
			}
		}
		if (blocks->at(i).mIsVisible == true && blocks->at(i).mIsSucked == true) {

			if (blocks->at(i).mFlyAngleTarget == -1.0f) {
				float dot = glm::dot(blocks->at(i).mFlyDirectionNormalized, glm::vec2(-1.0f, 0.0f));
				blocks->at(i).mFlyAngleTarget = glm::acos(dot);
			}

			float crossProduct = glm::cross(blocks->at(i).mFlyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

			if (crossProduct > 0) {
				blocks->at(i).mFlyAngleOrientation = -1.0f;
			}
			else if (crossProduct < 0) {
				blocks->at(i).mFlyAngleOrientation = 1.0f;
			}

			if (blocks->at(i).mFlyAngle < blocks->at(i).mFlyAngleTarget) {
				blocks->at(i).mFlyAngle += 1.0f * deltaTime;
			}
		}
		if ((blocks->at(i).mSprite.mVertexData.Position.x > AABBPos.x - AABBSize.x * 2) && (blocks->at(i).mSprite.mVertexData.Position.x < AABBPos.x + AABBSize.x * 2)) {
			if (RectVsRect(AABBPos, AABBSize, blocks->at(i).mSprite.mVertexData.Position, blocks->at(i).mSprite.mVertexData.Size) && blocks->at(i).mIsVisible == true && blocks->at(i).mIsSucked == false) {
				affectedBlocks.push_back({ i , glm::distance2(epicenterAABBPos, blocks->at(i).mSprite.mVertexData.Position) });
			}
		}
	}
		std::sort(affectedBlocks.begin(), affectedBlocks.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
			return a.second < b.second;
			});

		for (int i = 0; i < affectedBlocks.size(); i++) {
			blocks->at(affectedBlocks[i].first).mIsVisible = true;
			blocks->at(affectedBlocks[i].first).mIsCollidable = false;
			blocks->at(affectedBlocks[i].first).mIsSucked = true;
			blocks->at(affectedBlocks[i].first).mFlyDirectionNormalized = glm::normalize(epicenterAABBPos - blocks->at(affectedBlocks[i].first).mSprite.mVertexData.Position);
			blocks->at(affectedBlocks[i].first).mFlyVelocity = blocks->at(affectedBlocks[i].first).mFlyDirectionNormalized * (float)randomVelocity(gen);
		}
		affectedBlocks.clear();
	//*/

	if (isBorn) {
		float distance = glm::distance2(mSprite.mVertexData.Position + mSprite.mVertexData.Size / 2.0f, actor.mPosition + actor.mSprite.mVertexData.Size / 2.0f);

		if (distance < idleRadius) {
			idleVolume = 128.0f - (distance / idleRadius * 128.0f);
			Mix_Volume(13, idleVolume * globalSFXVolumeModifier);
			if (!Mix_Playing(13)) {
				Mix_PlayChannel(13, blackHoleIdle, 0);
			}
		}
		else {
			idleVolume = 0;
			Mix_Volume(13, idleVolume * globalSFXVolumeModifier);
		}
	}
}

void BlackHole::Reset() {
	isBorn = false;
	loopTimerOneShot = false;
	birthTimerOneShot = false;
	idleVolume = 0;
	AABBSize.x = 350.0f;
}