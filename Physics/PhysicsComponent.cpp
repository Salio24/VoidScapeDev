#include "PhysicsComponent.hpp"

PhysicsComponent::PhysicsComponent() {

}

PhysicsComponent::~PhysicsComponent() {

}

bool PhysicsComponent::GetDeath() {
	return death1;
}

void PhysicsComponent::FixedTickrateUpdate(double timeStep, const std::vector<GameObject>* blocks, bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], glm::vec2 colliderSize) {

	previous = current;
	MovementUpdate(activeKeys, timeStep);
	VSMath::RK4_IntegrateVec2(current, 0.0f, timeStep, acceleration);
	CollisionUpdate(blocks, colliderSize);

	//std::cout << normal.x << std::endl;
	
	std::cout << "Vel: " << glm::to_string(current.velocity) << " Pos: " << glm::to_string(current.position) << " Acc: " << glm::to_string(acceleration) << std::endl;
}

std::pair<glm::vec2, glm::vec2> PhysicsComponent::Update(double accumulator, double timeStep) {
	const double alpha = accumulator / timeStep;

	std::pair<glm::vec2, glm::vec2> interpolated;

	interpolated.first = VSMath::MultiplyVec2AndDouble(current.position, alpha) + VSMath::MultiplyVec2AndDouble(previous.position, (1.0f - alpha));
	interpolated.second = VSMath::MultiplyVec2AndDouble(current.velocity, alpha) + VSMath::MultiplyVec2AndDouble(previous.velocity, (1.0f - alpha));

	return interpolated;
}

void PhysicsComponent::SetPosition(glm::vec2 position) {
	current.position = position;
}

void PhysicsComponent::CollisionUpdate(const std::vector<GameObject>* blocks, glm::vec2 colliderSize) {

	normal = glm::vec2(0.0f);

	glm::vec2 baseCollderSize = colliderSize;

	bool death = false;

	if (mSliding || mCrouching) {
		colliderSize.y = colliderSize.y * mPhysicsSettings.SlidingCollidorFactor;
	}

	bool rightHugging = false;
	bool leftHugging = false;

	mCanStand = true;

	// Bottom left corner of broad-phase-box
	glm::vec2 A(current.position.x - 3 * colliderSize.x, current.position.y - 3 * colliderSize.y);
	// Top right corner of broad-phase-box
	glm::vec2 B(current.position.x + 4 * colliderSize.x, current.position.y + 4 * colliderSize.y);
	
	std::vector<int> bpb;
	
	for (int i = 0; i < blocks->size(); i++) {
		if (blocks->at(i).mIsCollidable || blocks->at(i).mIsDeathTrigger) {
			bpb.push_back(i);
		}
	}
	
	for (auto blockID : bpb) {
		if (blocks->at(blockID).mIsCollidable) {
			if (VSMath::Physics::ResolvePenetrationOnY(current.position, current.velocity, colliderSize, blocks->at(blockID).mSprite.mVertexData.Position, blocks->at(blockID).mSprite.mVertexData.Size, normal) && blocks->at(blockID).mIsDeathTrigger) {
				death = true;
			}
		}
	}
	for (auto blockID : bpb) {
		if (blocks->at(blockID).mIsCollidable) {
			if (VSMath::Physics::ResolvePenetrationOnX(current.position, current.velocity, colliderSize, blocks->at(blockID).mSprite.mVertexData.Position, blocks->at(blockID).mSprite.mVertexData.Size, normal) && blocks->at(blockID).mIsDeathTrigger) {
				death = true;
			}
			if (mWallHugRight && VSMath::Physics::RectVsRect(glm::vec2(current.position.x + colliderSize.x / 4, current.position.y), colliderSize, blocks->at(blockID).mSprite.mVertexData.Position, blocks->at(blockID).mSprite.mVertexData.Size)) {
				rightHugging = true;
			}
			else if (mWallHugLeft && VSMath::Physics::RectVsRect(glm::vec2(current.position.x - colliderSize.x / 4, current.position.y), colliderSize, blocks->at(blockID).mSprite.mVertexData.Position, blocks->at(blockID).mSprite.mVertexData.Size)) {
				leftHugging = true;
			}
		}
		if (VSMath::Physics::RectVsRect(current.position, baseCollderSize, blocks->at(blockID).mSprite.mVertexData.Position, blocks->at(blockID).mSprite.mVertexData.Size) && blocks->at(blockID).mIsCollidable) {
			mCanStand = false;
		}
		if (VSMath::Physics::RectVsRect(current.position, colliderSize, blocks->at(blockID).mSprite.mVertexData.Position, blocks->at(blockID).mSprite.mVertexData.Size) && blocks->at(blockID).mIsDeathTrigger) {
			death = true;
		}
	}

	if (normal.y == 1) {
		mGrounded = true;
	}
	else {
		mGrounded = false;
	} 

	if (!rightHugging && mWallHugRight) {
		mWallHugRight = false;
	}
	else if (!leftHugging && mWallHugLeft) {
		mWallHugLeft = false;
	}

	if (normal.x == -1 && current.velocity.y < 0.0f) {
		mWallHugRight = true;
	} 
	else if (normal.x == 1 && current.velocity.y < 0.0f) {
		mWallHugLeft = true;
	}
	death1 = death;
}

void PhysicsComponent::MovementUpdate(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], double timeStep) {
	acceleration = glm::vec2(0.0f, 0.0f);

	// Gravity
	if (current.velocity.y <= 0 && current.velocity.y > mPhysicsSettings.FallSpeedLimit) {
		acceleration.y += mPhysicsSettings.GravityDOWN;
	}
	else if (current.velocity.y > 0) {
		acceleration.y += mPhysicsSettings.GravityUP;
	}

	// Fall speed limit
	if (current.velocity.y < mPhysicsSettings.FallSpeedLimit) {
		acceleration.y += -(current.velocity.y - mPhysicsSettings.FallSpeedLimit) / timeStep;
	}

	if (Sign(current.velocity.x) == -1) {
		mLookDirection = LookDirections::LEFT;
	}
	else if (Sign(current.velocity.x) == 1) {
		mLookDirection = LookDirections::RIGHT;
	}

	if (!mCanStand) {
		mCrouching = true;
	}

	// Movement on X-axis (run) on ground and mid air
	if (activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)]) {
		if (current.velocity.x > -mPhysicsSettings.RunSpeedLimit && !mSliding && !mCrouching) {
			if (mGrounded) {
				// run
				if (current.velocity.x - mPhysicsSettings.RunAccelerationOnFoot * timeStep >= -mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += -mPhysicsSettings.RunAccelerationOnFoot;
				}
				else if (current.velocity.x < 0.0f) {
					acceleration.x += -(current.velocity.x + mPhysicsSettings.RunSpeedLimit) / timeStep;
				}
			}
			else {
				// mid air
				if (current.velocity.x - mPhysicsSettings.RunAccelerationMidAir * timeStep >= -mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += -mPhysicsSettings.RunAccelerationMidAir;
				}
				else if (current.velocity.x < 0.0f) {
					acceleration.x += -(current.velocity.x + mPhysicsSettings.RunSpeedLimit) / timeStep;
				}
			}
		}
		else if (current.velocity.x < -mPhysicsSettings.RunSpeedLimit && mGrounded) {
			acceleration.x += -(current.velocity.x + mPhysicsSettings.RunSpeedLimit) / timeStep;
		}

		// crouch
		if (current.velocity.x > -mPhysicsSettings.CrouchSpeedLimit && mCrouching) {
			if (current.velocity.x - mPhysicsSettings.CrouchAcceleration * timeStep >= -mPhysicsSettings.CrouchSpeedLimit) {
				acceleration.x += -mPhysicsSettings.CrouchAcceleration;
			}
			else if (current.velocity.x < 0.0f) {
				acceleration.x += -(current.velocity.x + mPhysicsSettings.CrouchSpeedLimit) / timeStep;
			}
		}
		else if (current.velocity.x < -mPhysicsSettings.CrouchSpeedLimit && mCrouching) {
			acceleration.x += -(current.velocity.x + mPhysicsSettings.CrouchSpeedLimit) / timeStep;
		}

		mLookDirection = LookDirections::LEFT;
	}
	if (activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) {
		if (current.velocity.x < mPhysicsSettings.RunSpeedLimit && !mSliding && !mCrouching) {
			if (mGrounded) {
				if (current.velocity.x + mPhysicsSettings.RunAccelerationOnFoot * timeStep <= mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += mPhysicsSettings.RunAccelerationOnFoot;
				}
				else if (current.velocity.x > 0.0f) {
					acceleration.x += -(current.velocity.x - mPhysicsSettings.RunSpeedLimit) / timeStep;
				}
			}
			else {
				if (current.velocity.x + mPhysicsSettings.RunAccelerationMidAir * timeStep <= mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += mPhysicsSettings.RunAccelerationMidAir;
				}
				else if (current.velocity.x > 0.0f) {
					acceleration.x += -(current.velocity.x - mPhysicsSettings.RunSpeedLimit) / timeStep;
				}
			}
		}
		else if (current.velocity.x > mPhysicsSettings.RunSpeedLimit && mGrounded) {
			acceleration.x += -(current.velocity.x - mPhysicsSettings.RunSpeedLimit) / timeStep;
		}

		if (current.velocity.x < mPhysicsSettings.CrouchSpeedLimit && mCrouching) {
			if (current.velocity.x + mPhysicsSettings.CrouchAcceleration * timeStep <= mPhysicsSettings.CrouchSpeedLimit) {
				acceleration.x += mPhysicsSettings.CrouchAcceleration;
			}
			else if (current.velocity.x > 0.0f) {
				acceleration.x += -(current.velocity.x - mPhysicsSettings.CrouchSpeedLimit) / timeStep;
			}
		}
		else if (current.velocity.x > mPhysicsSettings.CrouchSpeedLimit && mCrouching) {
			acceleration.x += -(current.velocity.x - mPhysicsSettings.CrouchSpeedLimit) / timeStep;
		}

		mLookDirection = LookDirections::RIGHT;
	}

	// for state machine
	mActiveRunning = false;
	mPassiveRunning = false;
	if ((activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)] || activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) && std::abs(current.velocity.x) > 0.0f && mGrounded && !mSliding && !mCrouching) {
		mActiveRunning = true;
	}
	else if (!(activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)] || activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) && std::abs(current.velocity.x) > 0.0f && mGrounded) {
		mPassiveRunning = true;
	}


	// Friction on gound and air resistance mid air
	if ((!(activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)] || activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) || mSliding) && std::abs(current.velocity.x) > 0.0f) {
		if (mGrounded) { // On ground
			if (mSliding) {
				if (std::abs(current.velocity.x) < mPhysicsSettings.SlidingFrictionStopSpeed) {
					// No input: stop the object when velocity is below threshold
					acceleration.x = -current.velocity.x / timeStep;
				}
				else {
					// Moving without input: apply constant friction	
					acceleration.x = -mPhysicsSettings.SlidingFriction * glm::normalize(current.velocity).x;
					// values > ~3800.0f are unstable 
				}
			}
			else {
				if (std::abs(current.velocity.x) < mPhysicsSettings.DefaultFrictionStopSpeed) {
					// No input: stop the object when velocity is below threshold
					acceleration.x = -current.velocity.x / timeStep;
				}
				else {
					// Moving without input: apply constant friction	
					acceleration.x = -mPhysicsSettings.DefaultFriction * glm::normalize(current.velocity).x;
					// values > ~3800.0f are unstable 
				}
			}
		}
		else {
			if (std::abs(current.velocity.x) < mPhysicsSettings.AirResistanceStopSpeed) {
				// No input: stop the object when velocity is below threshold
				acceleration.x = -current.velocity.x / timeStep;
			}
			else {
				// Moving without input: apply constant friction
				acceleration.x = -mPhysicsSettings.DefaultAirResistance * glm::normalize(current.velocity).x;
				// values > ~3800.0f are unstable
			}
		}
	}

	// Slide
	mFastWallSlide = false;
	if (activeKeys[static_cast<int>(ActiveKeys::DUCK)]) {
		if (mDuckOneShot && mGrounded) {
			

			switch (mLookDirection) {
			case LookDirections::LEFT:
				if (current.velocity.x < -mPhysicsSettings.ThresholdSpeedToSlide) {
					acceleration.x += mPhysicsSettings.SlidingImpulse * Sign(current.velocity.x);

					mSliding = true;
				}
				else if (current.velocity.x > -mPhysicsSettings.ThresholdSpeedToSlide && current.velocity.x <= 0.0f) {
					mCrouching = true;
				}
				
				
				break;
			case LookDirections::RIGHT:
				if (current.velocity.x > mPhysicsSettings.ThresholdSpeedToSlide) {
					acceleration.x += mPhysicsSettings.SlidingImpulse * Sign(current.velocity.x);

					mSliding = true;
				}
				else if (current.velocity.x < mPhysicsSettings.ThresholdSpeedToSlide && current.velocity.x >= 0.0f) {
					mCrouching = true;
				}

				break;
			default:
				break;
			}
			mDuckOneShot = false;
		} 
		else if (mWallHugRight || mWallHugLeft) {
			mFastWallSlide = true;
		}
	}
	else {
		if (mCanStand) {
			mCrouching = false;
		}
		mSliding = false;
	}
	if (mSliding && std::abs(current.velocity.x) < 250.0f) {
		mSliding = false;
		mCrouching = true;
	}

	// coyote tome
	if (mGrounded) {
		coyoteTimeTimer = 0;
	}
	else if (coyoteTimeTimer < 500) {
		coyoteTimeTimer++;
	}

	if (coyoteTimeTimer < mPhysicsSettings.CoyoteTimeTicks && !mJumping && !mGrounded) {
		mCoyoteTimeActive = true;
	}
	else {
		mCoyoteTimeActive = false;
	}

	// jumps etc
	if (activeKeys[static_cast<int>(ActiveKeys::SPACE)]) {
		if (mDoubleJumping && doubleJumpTickTimer < mPhysicsSettings.VariableDoubleJumpTicks) {
			doubleJumpTickTimer++;
			acceleration.y += mPhysicsSettings.DoubleJumpContinuousAcceleration;
		} else if (mWallJumping && wallJumpTickTimer < mPhysicsSettings.VariableWallJumpTicks) {
			wallJumpTickTimer++;
			acceleration.y += mPhysicsSettings.WallJumpContinuousAcceleration;
		}

		if (mSpacebarOneShot) {

			if (!(mWallHugLeft || mWallHugRight)) {
				if (!mGrounded && mDoubleJumpAvailable && !mCoyoteTimeActive) {
					mDoubleJumpAvailable = false;
					mDoubleJumping = true;
					doubleJumpTickTimer = 0;
					acceleration.y = -current.velocity.y / timeStep;
					acceleration.y += mPhysicsSettings.DoubleJumpStartImpulse;
				}
				else {
					jumpBufferTimer = 0;
				}
			}
			else {
				mWallJumping = true;

				wallJumpTickTimer = 0;

				acceleration.y = -current.velocity.y / timeStep;
				acceleration.y += mPhysicsSettings.WallJumpStartImpulse;
				if (mWallHugRight) {
					acceleration.x += -mPhysicsSettings.WallJumpStartSideImpulse;
				}
				else if (mWallHugLeft) {
					acceleration.x += mPhysicsSettings.WallJumpStartSideImpulse;
				}

				mWallHugRight = false;
				mWallHugLeft = false;
			}

			mSpacebarOneShot = false;
		}

		if (mJumping && jumpTickTimer < mPhysicsSettings.VariableJumpTicks) {
			jumpTickTimer++;
			acceleration.y += mPhysicsSettings.JumpContinuousAcceleration;
		}
	}
	else {
		mJumping = false;
		mDoubleJumping = false;
		mWallJumping = false;
	}

	if (mWallHugRight) {
		mDoubleJumpAvailable = true;

		if (!mFastWallSlide) {
			acceleration.y = -(current.velocity.y + mPhysicsSettings.WallSlideSpeed) / timeStep;
		}

		if (current.velocity.x < 0.0f) {
			mWallHugRight = false;
		}
	}
	else if (mWallHugLeft) {
		mDoubleJumpAvailable = true;

		if (!mFastWallSlide) {
			acceleration.y = -(current.velocity.y + mPhysicsSettings.WallSlideSpeed) / timeStep;
		}

		if (current.velocity.x > 0.0f) {
			mWallHugLeft = false;
		}
	}

	if ((mGrounded || mCoyoteTimeActive) && jumpBufferTimer < mPhysicsSettings.JumpBufferTicks && mCanStand) {
		if (mCoyoteTimeActive) {
			coyoteTimeTimer += 1000;
			acceleration.y = -(current.velocity.y + mPhysicsSettings.WallSlideSpeed) / timeStep;
		}

		jumpBufferTimer += 1000.0f;
		mJumping = true;

		jumpTickTimer = 0;

		acceleration.y += mPhysicsSettings.JumpStartImpulse;
		mGrounded = false;
	}

	jumpBufferTimer++;

	if (mGrounded) {
		mJumping = false;
		mDoubleJumpAvailable = true;
		mWallHugLeft = false;
		mWallHugRight = false;
	}
	if (current.velocity.y < 0.0f) {
		if (jumpTickTimer != 0) {
			mJumping = false;
		}
		else if (doubleJumpTickTimer != 0) {
			mDoubleJumping = false;
		}
		else if (wallJumpTickTimer != 0) {
			mWallJumping = false;
		}
	}
	if (!mGrounded) {
		mSliding = false;
		mCrouching = false;
	}

	// todo, global speed limits
}