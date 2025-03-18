#include "PhysicsComponent.hpp"

PhysicsComponent::PhysicsComponent() {

}

PhysicsComponent::~PhysicsComponent() {

}

void PhysicsComponent::FixedTickrateUpdate(double timeStep, const std::vector<GameObject>* blocks, bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], glm::vec2 colliderSize) {

	// add 2 distinct structs, static box and dynamic box

	// separate integration, collision function (functions that are not tightly bond to acter), into a separate file, and make them static

	previous = current;
	MovementUpdate(activeKeys, timeStep);
	VSMath::RK4_IntegrateVec2(current, 0.0f, timeStep, acceleration);
	CollisionUpdate(blocks, colliderSize);


	if (current.position.y > 640.0f) {

	}
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

	bool rightHugging = false;
	bool leftHugging = false;

	// Bottom left corner of broad-phase-box
	glm::vec2 A(current.position.x - 3 * colliderSize.x, current.position.y - 3 * colliderSize.y);
	// Top right corner of broad-phase-box
	glm::vec2 B(current.position.x + 4 * colliderSize.x, current.position.y + 4 * colliderSize.y);
	
	for (const auto& block : *blocks) {
		if (!block.mIsDeathTrigger && block.mIsCollidable && block.mSprite.mVertexData.Position.x > A.x && block.mSprite.mVertexData.Position.x < B.x && block.mSprite.mVertexData.Position.y > A.y && block.mSprite.mVertexData.Position.y < B.y) {
			VSMath::Physics::ResolvePenetrationOnY(current.position, current.velocity, colliderSize, block.mSprite.mVertexData.Position, block.mSprite.mVertexData.Size, normal);
		}
	}
	for (const auto& block : *blocks) {
		if (!block.mIsDeathTrigger && block.mIsCollidable && block.mSprite.mVertexData.Position.x > A.x && block.mSprite.mVertexData.Position.x < B.x && block.mSprite.mVertexData.Position.y > A.y && block.mSprite.mVertexData.Position.y < B.y) {
			VSMath::Physics::ResolvePenetrationOnX(current.position, current.velocity, colliderSize, block.mSprite.mVertexData.Position, block.mSprite.mVertexData.Size, normal);
			if (mWallHugRight && VSMath::Physics::RectVsRect(glm::vec2(current.position.x + colliderSize.x / 4, current.position.y), colliderSize, block.mSprite.mVertexData.Position, block.mSprite.mVertexData.Size)) {
				rightHugging = true;
			}
			else if (mWallHugLeft && VSMath::Physics::RectVsRect(glm::vec2(current.position.x - colliderSize.x / 4, current.position.y), colliderSize, block.mSprite.mVertexData.Position, block.mSprite.mVertexData.Size)) {
				leftHugging = true;
			}
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
		std::cout << "aaa" << std::endl;
	}

	if (normal.x == -1) {
		mWallHugRight = true;
	} 
	else if (normal.x == 1) {
		mWallHugLeft = true;
	}

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

	// Movement on X-axis (run) on ground and mid air
	if (activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)]) {
		if (current.velocity.x > -mPhysicsSettings.RunSpeedLimit && !mSliding) {
			if (mGrounded) {
				if (current.velocity.x - mPhysicsSettings.RunAccelerationOnFoot * timeStep >= -mPhysicsSettings.RunSpeedLimit) {
					acceleration.x += -mPhysicsSettings.RunAccelerationOnFoot;
				}
				else if (current.velocity.x < 0.0f) {
					acceleration.x += -(current.velocity.x + mPhysicsSettings.RunSpeedLimit) / timeStep;
				} 
			}
			else {
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
		mLookDirection = LookDirections::LEFT;
	}
	if (activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) {
		if (current.velocity.x < mPhysicsSettings.RunSpeedLimit && !mSliding) {
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
		mLookDirection = LookDirections::RIGHT;
	}

	mActiveRunning = false;
	mPassiveRunning = false;
	if ((activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)] || activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) && std::abs(current.velocity.x) > 0.0f && mGrounded) {
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
	if (activeKeys[static_cast<int>(ActiveKeys::DUCK)]) {
		if (mDuckOneShot && mGrounded) {
			

			switch (mLookDirection) {
			case LookDirections::LEFT:
				if (current.velocity.x < -mPhysicsSettings.ThresholdSpeedToSlide) {
					acceleration.x += mPhysicsSettings.SlidingImpulse * Sign(current.velocity.x);

					mSliding = true;
				}
				
				
				break;
			case LookDirections::RIGHT:
				if (current.velocity.x > mPhysicsSettings.ThresholdSpeedToSlide) {
					acceleration.x += mPhysicsSettings.SlidingImpulse * Sign(current.velocity.x);

					mSliding = true;
				}


				break;
			default:
				break;
			}
			mDuckOneShot = false;
		} 
		else if (mWallHugRight || mWallHugLeft) {
			mWallHugRight = false;
			mWallHugLeft = false;
		}
	}
	else {
		mSliding = false;
	}

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
				if (!mGrounded && mDoubleJumpAvailable) {
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

		acceleration.y = -(current.velocity.y + mPhysicsSettings.WallSlideSpeed) / timeStep;
		if (current.velocity.x < 0.0f) {
			mWallHugRight = false;
		}
	}
	else if (mWallHugLeft) {
		mDoubleJumpAvailable = true;

		acceleration.y = -(current.velocity.y + mPhysicsSettings.WallSlideSpeed) / timeStep;
		if (current.velocity.x > 0.0f) {
			mWallHugLeft = false;
		}
	}

	if (mWallJumping) {
	}

	if (mGrounded && jumpBufferTimer < mPhysicsSettings.JumpBufferTicks) {
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
	}
	if (testButton3) {
		
	}

	// todo, global speed limits
}