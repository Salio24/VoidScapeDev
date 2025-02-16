#include "MovementHandler.hpp"

MovementHandler::MovementHandler() {
}

MovementHandler::~MovementHandler() {
}

void MovementHandler::Move(glm::vec2& actorVelocity, const glm::vec2& acceleration) {
	actorVelocity = glm::vec2(actorVelocity.x + acceleration.x, actorVelocity.y + acceleration.y);
}

void MovementHandler::Jump(float& deltaTime, const float& jumpSpeed, glm::vec2& actorVelocity) {
	actorVelocity.y += jumpSpeed * deltaTime;
}

void MovementHandler::Slam(float& deltaTime, const float& slamSpeed, const float& speedLimit, glm::vec2& actorVelocity) {
	actorVelocity.y -= slamSpeed * deltaTime;

	if (actorVelocity.y < -speedLimit) {
		actorVelocity.y = -speedLimit;
	}

}

void MovementHandler::FinishDoubleJump() {
	doubleJumpTimer = std::chrono::time_point<std::chrono::steady_clock>{};
}

void MovementHandler::FinishJump() {
	jumpTimer = std::chrono::time_point<std::chrono::steady_clock>{};
}

void MovementHandler::Update(float& deltaTime, Actor& actor, const bool& started, Config& config) {
	if ((!actor.mIsSucked && !actor.mConsumedByBlackHole && !actor.mEscaped && !actor.mIsSuckedPortal) && started) {

		mLast_mCanDoubleJump = mCanDoubleJump;
		// Gravity and wall stick vvv
		if (mCanWallStick) {
			actor.mVelocity.y = config.ConstantWallSlideSpeed;
		}

		if (actor.mVelocity.y <= 0.0f) {
			actor.mVelocity.y += config.GravityAccelerationWhenGoingDown * deltaTime;
		}
		else if (actor.mVelocity.y > 0.0f) {
			actor.mVelocity.y += config.GravityAccelerationWhenGoingUp * deltaTime;
		}
		// Gravity and wall stick ^^^

		// Run movement vvv
		if (mKeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] && !actor.mDead) {
			if (mIsGrounded) {
				if (!(actor.mVelocity.x + -config.RunAccelerationOnFoot * deltaTime < -config.RunSpeedLimitOnFoot)) {
					Move(actor.mVelocity, glm::vec2(-config.RunAccelerationOnFoot, 0.0f) * deltaTime);
				}
			}
			else {
				if (!(actor.mVelocity.x + -config.RunAccelerationMidAir * deltaTime < -config.RunSpeedLimitMidAir)) {
					Move(actor.mVelocity, glm::vec2(-config.RunAccelerationMidAir, 0.0f) * deltaTime);
				}
			}
			mLookDirection = LookDirections::LEFT;
		}
		if (mKeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] && !actor.mDead) {
			if (mIsGrounded) {
				if (!(actor.mVelocity.x + config.RunAccelerationOnFoot * deltaTime > config.RunSpeedLimitOnFoot)) {
					Move(actor.mVelocity, glm::vec2(config.RunAccelerationOnFoot, 0.0f) * deltaTime);
				}
			}
			else {
				if (!(actor.mVelocity.x + config.RunAccelerationMidAir * deltaTime > config.RunSpeedLimitMidAir)) {
					Move(actor.mVelocity, glm::vec2(config.RunAccelerationMidAir, 0.0f) * deltaTime);
				}
			}
			mLookDirection = LookDirections::RIGHT;
		}
		// Run movement ^^^

		if (Sign(actor.mVelocity.x) == -1) {
			mLookDirection = LookDirections::LEFT;
		}
		else if (Sign(actor.mVelocity.x) == 1) {
			mLookDirection = LookDirections::RIGHT;
		}


		// Slam and slide vvv
		if (mKeyboadStates[static_cast<int>(MovementState::DUCK)] && !actor.mDead) {

			if (mDuckOneShot) {
				if (!mIsGrounded) { 
					mIsSlamming = true;
					mCanDoubleJump = false;
					mDuckOneShot = false;
				}
				if (mIsGrounded && (actor.mVelocity.x > config.MinSpeedToStartSlide || actor.mVelocity.x < -config.MinSpeedToStartSlide)) { // use abs value of vel when refactoring instead of ||
					slideDirection = Sign(actor.mVelocity.x);
					mIsSliding = true;
					mDuckOneShot = false;
				}
			}
			
		}


		if (!mKeyboadStates[static_cast<int>(MovementState::DUCK)] || !mIsGrounded) {
			slideOneShot = true;
		}

		if (mKeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] && slideDirection == 1 && !actor.mDead) {
			mIsSliding = false;
		}

		if (mKeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] && slideDirection == -1 && !actor.mDead) {
			mIsSliding = false;
		}


		if (!mKeyboadStates[static_cast<int>(MovementState::DUCK)]) {
			frictionModifier = config.DefaultFriction;
			mIsSliding = false; 
		}


		if (mIsSlamming) {
			Slam(deltaTime, config.SlamAcceleartion, config.SlamSpeedLimit, actor.mVelocity);
		}

		if (mIsSliding) {
			if (slideOneShot && mIsGrounded) {
				frictionModifier = config.SlidingFriction;
				actor.mVelocity.x = actor.mVelocity.x + config.SlideSpeedBoost * slideDirection;
				slideOneShot = false;
			}
		}
		

		if (mIsGrounded || (!mIsGrounded && mCanWallStick)) {
			mIsSlamming = false;
		}


		// Slam and slide ^^^

		// Jumping, wall jumping, double jumping vvv
		if (mKeyboadStates[static_cast<int>(MovementState::SPACE)] && !actor.mDead) {
			if (mSpacebarOneShot)
			{
				if (mCanWallStick) {
					isWallJumping = true;
				}
				
				jumpBufferTimer = std::chrono::high_resolution_clock::now();
				
				if (!mIsGrounded && !mCanWallStick && mCanDoubleJump) {
					mCanDoubleJump = false;
					actor.mVelocity.y = config.InitialSpeedBoostOnDoubleJump;
					doubleJumpTimer = std::chrono::high_resolution_clock::now();
				}
			}
			mSpacebarOneShot = false;

			// may be use elapsed movement as a limit?
			// but then speed might be altered
			// what if limit is 800 but elapsed is 799?

			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - doubleJumpTimer).count() < config.VariableDoubleJumpTime + deltaTime * 1000) {
				Jump(deltaTime, config.VariableDoubleJumpAcceleration, actor.mVelocity);
			}

			if ((isJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpTimer).count() < config.VariableJumpTime + deltaTime * 1000)) {
				Jump(deltaTime, config.VariableJumpAcceleration, actor.mVelocity);
			}

			if (isWallJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - wallJumpTimer).count() < config.VariableWallJumpTime + deltaTime * 1000) {
				Jump(deltaTime, config.VariableWallJumpAcceleration, actor.mVelocity);
			}
		}
		if (!mKeyboadStates[static_cast<int>(MovementState::SPACE)]) { 
			isJumping = false;
			isWallJumping = false;
		}
		
		if (mCanWallStick && isWallJumping) {
			mCanDoubleJump = true;
			wallJumpTimer = std::chrono::high_resolution_clock::now();
			actor.mVelocity.y = config.InitialSpeedBoostOnWallJumpX;
			if (mIsWallMountableL) {
				actor.mVelocity.x = config.InitialSpeedBoostOnWallJumpY;
			}
			else if (mIsWallMountableR) {
				actor.mVelocity.x = -config.InitialSpeedBoostOnWallJumpY;
			}
			mIsWallMountableL = false;
			mIsWallMountableR = false;
		}

		if (((mIsWallMountableR && mRightWallHug) || (mIsWallMountableL && mLeftWallHug)) && !(mKeyboadStates[static_cast<int>(MovementState::SPACE)] && actor.mVelocity.y > 0.0f)) {
			mCanWallStick = true;
			mCanDoubleJump = true;
		}
		else {
			mCanWallStick = false;
		}

		if (mIsGrounded 
				&& std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpBufferTimer).count() < config.JumpBufferTime + deltaTime * 1000) {
			jumpBufferTimer = std::chrono::time_point<std::chrono::steady_clock>{};
			mCanDoubleJump = true;
			isJumping = true;
			jumpTimer = std::chrono::high_resolution_clock::now();
			actor.mVelocity.y = config.InitialSpeedBoostOnJump;
			mIsGrounded = false;
		}

		if (mIsGrounded) {
			mCanDoubleJump = true;
			isJumping = false;
		}

		if ((mIsWallMountableR && mRightWallHug) || (mIsWallMountableL && mLeftWallHug)) {
			isWallJumping = false;
		}

		// Jumping, wall jumping, double jumping ^^^

		// Friction and limits vvvq

		if (mIsGrounded) {
			float nextActorVelocityX = actor.mVelocity.x *= 1.0f - (frictionModifier * deltaTime);
			if (nextActorVelocityX > config.MinimalSpeedWithFriction || nextActorVelocityX < -config.MinimalSpeedWithFriction) {
				actor.mVelocity.x = nextActorVelocityX;
			}
			else {
				actor.mVelocity.x = 0.0f;
			}
		}
		if (actor.mVelocity.x > config.SpeedLiminOnX) {
			actor.mVelocity.x = config.SpeedLiminOnX;
		}
		if (actor.mVelocity.x < -config.SpeedLiminOnX) {
			actor.mVelocity.x = -config.SpeedLiminOnX;
		}
		if (actor.mVelocity.y < config.SpeedLiminOnY) {
			actor.mVelocity.y = config.SpeedLiminOnY;
		}

		// Friction and limits ^^^
	}
} 