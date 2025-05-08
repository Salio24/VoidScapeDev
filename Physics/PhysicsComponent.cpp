
#include "PhysicsComponent.hpp"
#include <mutex>

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
	
	death1 = death;

    std::cout << "DEBUG COLLISION END: normal.x=" << normal.x << " normal.y=" << normal.y << std::endl; // Add this
}

void PhysicsComponent::MovementUpdate(bool activeKeys[static_cast<int>(ActiveKeys::DUCK)], double timeStep) {
    acceleration = glm::vec2(0.0f, 0.0f);

    // --- Wall Interaction State Determination (using 'normal' from PREVIOUS CollisionUpdate) ---
    bool isTouchingWallLeftPrevFrame = (normal.x == 1.0f);  // Normal points away from wall: wall on left, normal points right
    bool isTouchingWallRightPrevFrame = (normal.x == -1.0f); // Wall on right, normal points left

    // Reset wall state if not actively wall jumping, or if grounded
    if (mGrounded || mWallJumping) { // If we become grounded, or are in a wall jump, not sliding/touching.
        mWallInteractionState = PlayerWallInteractionState::NONE;
    }
    // If already in a wall jump, mWallJumping is true, so we don't immediately re-evaluate wall slide state.
    // If we are not mWallJumping, then we can evaluate.
    if (!mWallJumping && !mGrounded) {
    if (isTouchingWallLeftPrevFrame && current.velocity.y <= 0 && activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)]) {
        // Only set to SLIDING if actively pressing into the wall
        mWallInteractionState = PlayerWallInteractionState::SLIDING_LEFT;
    } else if (isTouchingWallRightPrevFrame && current.velocity.y <= 0 && activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) {
        // Only set to SLIDING if actively pressing into the wall
        mWallInteractionState = PlayerWallInteractionState::SLIDING_RIGHT;
    } else {
        // If not actively pressing into a wall they are touching, or not touching, or moving up
        // they are not SLIDING. They might still be CAN_JUMP_OR_SLIDE if touching,
        // but for jump logic we'll directly use isTouchingWall...PrevFrame.
        // Let's simplify mWallInteractionState to only handle active sliding or none for now.
        if (mWallInteractionState == PlayerWallInteractionState::SLIDING_LEFT || mWallInteractionState == PlayerWallInteractionState::SLIDING_RIGHT) {
             // Only transition out of sliding if conditions are no longer met
            if (!isTouchingWallLeftPrevFrame && mWallInteractionState == PlayerWallInteractionState::SLIDING_LEFT) mWallInteractionState = PlayerWallInteractionState::NONE;
            if (!isTouchingWallRightPrevFrame && mWallInteractionState == PlayerWallInteractionState::SLIDING_RIGHT) mWallInteractionState = PlayerWallInteractionState::NONE;
            if (current.velocity.y > 0) mWallInteractionState = PlayerWallInteractionState::NONE; // if moving up, not sliding down
        }
        // If not previously sliding, and not meeting slide conditions now, remain NONE or transition out.
        if(mWallInteractionState != PlayerWallInteractionState::SLIDING_LEFT && mWallInteractionState != PlayerWallInteractionState::SLIDING_RIGHT) {
            mWallInteractionState = PlayerWallInteractionState::NONE;
        }
    }
} else { // If grounded or already wall jumping, ensure not in any other wall state
    mWallInteractionState = PlayerWallInteractionState::NONE;
}


    // --- Gravity ---
    if (current.velocity.y <= 0 && current.velocity.y > mPhysicsSettings.FallSpeedLimit) {
        acceleration.y += mPhysicsSettings.GravityDOWN;
    } else if (current.velocity.y > 0) {
        acceleration.y += mPhysicsSettings.GravityUP;
    }
    // Fall speed limit will be applied later, AFTER specific states like wall sliding modify acceleration

    // --- Look Direction ---
    if (Sign(current.velocity.x) == -1) {
        mLookDirection = LookDirections::LEFT;
    } else if (Sign(current.velocity.x) == 1) {
        mLookDirection = LookDirections::RIGHT;
    }

    // --- Crouching / Standing ---
    if (!mCanStand) { // mCanStand is set in CollisionUpdate if something is above the player
        mCrouching = true;
    }
    // Note: mCrouching can also be set by ducking logic further down.

    // --- Fast Wall Slide Preparation (from DUCK key) ---
    mFastWallSlide = false; // Reset per frame
    if (activeKeys[static_cast<int>(ActiveKeys::DUCK)]) {
        if (!mGrounded && (mWallInteractionState == PlayerWallInteractionState::SLIDING_LEFT ||
                           mWallInteractionState == PlayerWallInteractionState::SLIDING_RIGHT ||
                           mWallInteractionState == PlayerWallInteractionState::CAN_JUMP_OR_SLIDE_LEFT ||
                           mWallInteractionState == PlayerWallInteractionState::CAN_JUMP_OR_SLIDE_RIGHT) ) {
            mFastWallSlide = true;
        }
    }


    // --- Wall Slide Physics ---
    if (mWallInteractionState == PlayerWallInteractionState::SLIDING_LEFT ||
        mWallInteractionState == PlayerWallInteractionState::SLIDING_RIGHT) {
        mGrounded = false;      // Cannot be grounded if wall sliding
        mJumping = false;       // Stop regular jump if we start sliding
        mDoubleJumping = false; // Stop double jump
        mCoyoteTimeActive = false; // No coyote time on wall
        coyoteTimeTimer = mPhysicsSettings.CoyoteTimeTicks + 1; // Expire coyote time

        mDoubleJumpAvailable = true; // Replenish double jump on wall slide

        if (!mFastWallSlide) {
            // Apply wall slide speed: try to set Y velocity to -WallSlideSpeed
            // This counteracts gravity to achieve the target slide speed.
            if (current.velocity.y < -mPhysicsSettings.WallSlideSpeed) {
                 acceleration.y += (-mPhysicsSettings.WallSlideSpeed - current.velocity.y) / timeStep;
            } else if (current.velocity.y > -mPhysicsSettings.WallSlideSpeed && current.velocity.y <=0) {
                // If falling slower or stationary, we need to ensure gravity doesn't accelerate beyond wall slide speed.
                // The gravity is already applied. We add an opposing force.
                // The original: acceleration.y = -(current.velocity.y + mPhysicsSettings.WallSlideSpeed) / timeStep;
                // This targets velocity. Let's use this one.
                acceleration.y = -(current.velocity.y + mPhysicsSettings.WallSlideSpeed) / timeStep;
            }
            // If current.velocity.y > 0 (moving up wall), gravity handles it, no slide speed needed.
        }
        // If mFastWallSlide is true, regular gravity applies (making the slide faster).
    }


    // --- Horizontal Movement (Run) ---
    // (Only if not wall jumping, as wall jump controls X acceleration)
    if (!mWallJumping) {
        if (activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)]) {
            if (current.velocity.x > -mPhysicsSettings.RunSpeedLimit && !mSliding && !mCrouching) {
                float accel = mGrounded ? mPhysicsSettings.RunAccelerationOnFoot : mPhysicsSettings.RunAccelerationMidAir;
                if (current.velocity.x - accel * timeStep >= -mPhysicsSettings.RunSpeedLimit) {
                    acceleration.x += -accel;
                } else if (current.velocity.x < 0.0f) { // Cap speed
                    acceleration.x += -(current.velocity.x + mPhysicsSettings.RunSpeedLimit) / timeStep;
                }
            } else if (current.velocity.x < -mPhysicsSettings.RunSpeedLimit && mGrounded && !mSliding && !mCrouching) { // Speed cap if already over
                acceleration.x += -(current.velocity.x + mPhysicsSettings.RunSpeedLimit) / timeStep;
            }

            if (mCrouching && current.velocity.x > -mPhysicsSettings.CrouchSpeedLimit) { // Crouch movement
                if (current.velocity.x - mPhysicsSettings.CrouchAcceleration * timeStep >= -mPhysicsSettings.CrouchSpeedLimit) {
                    acceleration.x += -mPhysicsSettings.CrouchAcceleration;
                } else if (current.velocity.x < 0.0f) {
                    acceleration.x += -(current.velocity.x + mPhysicsSettings.CrouchSpeedLimit) / timeStep;
                }
            } else if (mCrouching && current.velocity.x < -mPhysicsSettings.CrouchSpeedLimit) {
                 acceleration.x += -(current.velocity.x + mPhysicsSettings.CrouchSpeedLimit) / timeStep;
            }
            // mLookDirection = LookDirections::LEFT; // Already handled by Sign logic
        }
        if (activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) {
            if (current.velocity.x < mPhysicsSettings.RunSpeedLimit && !mSliding && !mCrouching) {
                float accel = mGrounded ? mPhysicsSettings.RunAccelerationOnFoot : mPhysicsSettings.RunAccelerationMidAir;
                if (current.velocity.x + accel * timeStep <= mPhysicsSettings.RunSpeedLimit) {
                    acceleration.x += accel;
                } else if (current.velocity.x > 0.0f) { // Cap speed
                    acceleration.x += -(current.velocity.x - mPhysicsSettings.RunSpeedLimit) / timeStep;
                }
            } else if (current.velocity.x > mPhysicsSettings.RunSpeedLimit && mGrounded && !mSliding && !mCrouching) { // Speed cap
                acceleration.x += -(current.velocity.x - mPhysicsSettings.RunSpeedLimit) / timeStep;
            }

            if (mCrouching && current.velocity.x < mPhysicsSettings.CrouchSpeedLimit) { // Crouch movement
                if (current.velocity.x + mPhysicsSettings.CrouchAcceleration * timeStep <= mPhysicsSettings.CrouchSpeedLimit) {
                    acceleration.x += mPhysicsSettings.CrouchAcceleration;
                } else if (current.velocity.x > 0.0f) {
                    acceleration.x += -(current.velocity.x - mPhysicsSettings.CrouchSpeedLimit) / timeStep;
                }
            } else if (mCrouching && current.velocity.x > mPhysicsSettings.CrouchSpeedLimit) {
                 acceleration.x += -(current.velocity.x - mPhysicsSettings.CrouchSpeedLimit) / timeStep;
            }
            // mLookDirection = LookDirections::RIGHT; // Already handled
        }
    }

    // --- Animation State Flags (Running) ---
    mActiveRunning = (activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)] || activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) && std::abs(current.velocity.x) > 0.0f && mGrounded && !mSliding && !mCrouching;
    mPassiveRunning = !(activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)] || activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) && std::abs(current.velocity.x) > 0.0f && mGrounded && !mSliding && !mCrouching;


    // --- Friction / Air Resistance (if not actively moving L/R or if sliding on ground) ---
    // (Only if not wall jumping, as wall jump has specific X trajectory)
    if (!mWallJumping && ((!(activeKeys[static_cast<int>(ActiveKeys::MOVE_LEFT)] || activeKeys[static_cast<int>(ActiveKeys::MOVE_RIGHT)]) || mSliding /*ground slide*/) && std::abs(current.velocity.x) > 0.0f)) {
        if (mGrounded) {
            float stopSpeed = mSliding ? mPhysicsSettings.SlidingFrictionStopSpeed : mPhysicsSettings.DefaultFrictionStopSpeed;
            float friction = mSliding ? mPhysicsSettings.SlidingFriction : mPhysicsSettings.DefaultFriction;
            if (std::abs(current.velocity.x) < stopSpeed) {
                acceleration.x = -current.velocity.x / timeStep;
            } else {
                acceleration.x = -friction * glm::normalize(current.velocity).x;
            }
        } else { // Mid-air
            if (std::abs(current.velocity.x) < mPhysicsSettings.AirResistanceStopSpeed) {
                acceleration.x = -current.velocity.x / timeStep;
            } else {
                acceleration.x = -mPhysicsSettings.DefaultAirResistance * glm::normalize(current.velocity).x;
            }
        }
    }

    // --- Ground Slide & Crouch from DUCK key ---
    if (activeKeys[static_cast<int>(ActiveKeys::DUCK)]) {
        if (mDuckOneShot && mGrounded) { // Initiate ground slide or crouch
            bool startedSlide = false;
            if (mLookDirection == LookDirections::LEFT && current.velocity.x < -mPhysicsSettings.ThresholdSpeedToSlide) {
                acceleration.x += mPhysicsSettings.SlidingImpulse * Sign(current.velocity.x);
                mSliding = true; startedSlide = true;
            } else if (mLookDirection == LookDirections::RIGHT && current.velocity.x > mPhysicsSettings.ThresholdSpeedToSlide) {
                acceleration.x += mPhysicsSettings.SlidingImpulse * Sign(current.velocity.x);
                mSliding = true; startedSlide = true;
            }
            
            if (!startedSlide) { // If not fast enough to slide, or wrong direction, just crouch
                mCrouching = true;
            }
            mDuckOneShot = false;
        }
        // mFastWallSlide logic is handled earlier
    } else { // DUCK key not pressed
        if (mCanStand) {
            mCrouching = false;
        }
        mSliding = false; // Stop ground slide if DUCK released
        mDuckOneShot = true; // Re-arm DUCK one-shot
    }

    if (mSliding && std::abs(current.velocity.x) < 250.0f && mGrounded) { // Transition from ground slide to crouch if slow enough
        mSliding = false;
        mCrouching = true;
    }
    if (!mGrounded) { // Cannot be ground-sliding or ground-crouching in air
        mSliding = false;
        if (mCanStand) mCrouching = false; // only uncrouch if can stand
    }


    // --- Coyote Time ---
    if (mGrounded) {
        coyoteTimeTimer = 0;
    } else if (mWallInteractionState == PlayerWallInteractionState::NONE && !mJumping && !mDoubleJumping && !mWallJumping) { // Only if truly airborne and not wall interacting/jumping
        if (coyoteTimeTimer < mPhysicsSettings.CoyoteTimeTicks) {
             coyoteTimeTimer++;
        }
    } else { // If on wall or jumping, expire coyote time
        coyoteTimeTimer = mPhysicsSettings.CoyoteTimeTicks + 1;
    }
    mCoyoteTimeActive = (coyoteTimeTimer < mPhysicsSettings.CoyoteTimeTicks);

    std::cout << "DEBUG JUMP CHECK: spaceShot=" << mSpacebarOneShot
          << " grounded=" << mGrounded
          << " wallJump=" << mWallJumping
          << " touchL=" << isTouchingWallLeftPrevFrame
          << " touchR=" << isTouchingWallRightPrevFrame
          << " wallState=" << static_cast<int>(mWallInteractionState)
          << " vel.y=" << current.velocity.y
          << " normal.x=" << normal.x // from previous frame
          << std::endl;
    // --- Jump Logic (Prioritized: Wall Jump > Double Jump > Regular Jump) ---
    if (activeKeys[static_cast<int>(ActiveKeys::SPACE)]) {
        if (mSpacebarOneShot && !mGrounded && (isTouchingWallLeftPrevFrame || isTouchingWallRightPrevFrame)) {
            // ---- WALL JUMP INITIATION ----
            mWallJumping = true;    // Set state to wall jumping
            mJumping = false;       // Ensure not in regular jump state
            mDoubleJumping = false; // Ensure not in double jump state
            mWallInteractionState = PlayerWallInteractionState::NONE; // Player is jumping OFF the wall, so not interacting anymore

            wallJumpTickTimer = 0; // Reset timer for continuous wall jump

            current.velocity.y = 0; // Reset Y velocity for a consistent wall jump upward impulse
            acceleration.y = 0;     // Clear any existing Y acceleration (like gravity from this frame)
            acceleration.y += mPhysicsSettings.WallJumpStartImpulse; // Apply wall jump upward impulse

            current.velocity.x = 0; // Reset X velocity for consistent push-off
            acceleration.x = 0;     // Clear any existing X acceleration
            if (isTouchingWallRightPrevFrame) { // If wall was on player's right, jump left
                acceleration.x += -mPhysicsSettings.WallJumpStartSideImpulse;
            } else if (isTouchingWallLeftPrevFrame) { // If wall was on player's left, jump right
                acceleration.x += mPhysicsSettings.WallJumpStartSideImpulse;
            }
            // If somehow both were true (corner), this prioritizes right wall contact.
            // Could also be just `else` if only one can be true.

            mGrounded = false;          // Player is now airborne
            mCoyoteTimeActive = false;  // No coyote time after a wall jump
            coyoteTimeTimer = mPhysicsSettings.CoyoteTimeTicks + 1; // Expire coyote timer
            mSpacebarOneShot = false;   // Consume the one-shot for this jump action
        }
        else if (mSpacebarOneShot && !mGrounded && !mWallJumping && mDoubleJumpAvailable && !mCoyoteTimeActive) {
            // ---- DOUBLE JUMP INITIATION ----
            mDoubleJumping = true;
            mJumping = false;
            mDoubleJumpAvailable = false;
            doubleJumpTickTimer = 0;

            current.velocity.y = 0; // Reset Y velocity for consistent double jump
            acceleration.y = 0;
            acceleration.y += mPhysicsSettings.DoubleJumpStartImpulse;
            mSpacebarOneShot = false;
        }
        else if (mSpacebarOneShot && (mGrounded || mCoyoteTimeActive) && mCanStand && !mJumping && !mDoubleJumping && !mWallJumping) {
            // ---- REGULAR JUMP INITIATION (Ground or Coyote) ----
            mJumping = true;
            jumpTickTimer = 0;

            if (mCoyoteTimeActive) {
                coyoteTimeTimer = mPhysicsSettings.CoyoteTimeTicks + 1; // Consume coyote
            }
            if (mGrounded && current.velocity.y < 0) current.velocity.y = 0; // Stop downward motion if on ground

            acceleration.y = 0; // Clear existing Y acceleration
            acceleration.y += mPhysicsSettings.JumpStartImpulse; // Add impulse
            mGrounded = false;
            mSpacebarOneShot = false;
        }
        else if (mSpacebarOneShot) { // Space pressed but no jump initiated (e.g., in air, no DJ)
            jumpBufferTimer = 0; // Activate jump buffer
            mSpacebarOneShot = false; // Consume for buffer activation
        }

        // ---- CONTINUOUS JUMP (Held Space) ----
        if (mWallJumping && wallJumpTickTimer < mPhysicsSettings.VariableWallJumpTicks) {
            wallJumpTickTimer++;
            acceleration.y += mPhysicsSettings.WallJumpContinuousAcceleration;
        } else if (mDoubleJumping && doubleJumpTickTimer < mPhysicsSettings.VariableDoubleJumpTicks) {
            doubleJumpTickTimer++;
            acceleration.y += mPhysicsSettings.DoubleJumpContinuousAcceleration;
        } else if (mJumping && jumpTickTimer < mPhysicsSettings.VariableJumpTicks) {
            jumpTickTimer++;
            acceleration.y += mPhysicsSettings.JumpContinuousAcceleration;
        }

    } else { // Space key NOT pressed
        mJumping = false;
        mDoubleJumping = false;
        mWallJumping = false; // Stop all continuous jump phases
        mSpacebarOneShot = true; // Re-arm for next press
    }

    // --- Jump Buffer Activation ---
    if (jumpBufferTimer < mPhysicsSettings.JumpBufferTicks) {
        if ((mGrounded || mCoyoteTimeActive) && mCanStand && !mJumping && !mDoubleJumping && !mWallJumping) {
            // ---- JUMP FROM BUFFER ----
            mJumping = true; // Buffer always results in a regular jump
            jumpTickTimer = 0;

            if (mCoyoteTimeActive) coyoteTimeTimer = mPhysicsSettings.CoyoteTimeTicks + 1;
            if (mGrounded && current.velocity.y < 0) current.velocity.y = 0;

            acceleration.y = 0;
            acceleration.y += mPhysicsSettings.JumpStartImpulse;
            mGrounded = false;
            jumpBufferTimer = mPhysicsSettings.JumpBufferTicks + 1; // Consume buffer

            // If space is HELD during buffer activation, continuous jump will pick up next frame.
        }
        jumpBufferTimer++;
    }


    // --- Final State Adjustments & Fall Speed Limit ---
    if (mGrounded) {
        mJumping = false; mDoubleJumping = false; mWallJumping = false;
        mDoubleJumpAvailable = true;
        mWallInteractionState = PlayerWallInteractionState::NONE;
        coyoteTimeTimer = 0;
        jumpBufferTimer = mPhysicsSettings.JumpBufferTicks + 1; // Expire buffer if grounded
    }

    // Stop continuous jump force if falling (apex reached or passed)
    if (current.velocity.y < 0.0f) {
        if (mJumping && jumpTickTimer > 0) mJumping = false; // End continuous phase, not the jump action itself
        if (mDoubleJumping && doubleJumpTickTimer > 0) mDoubleJumping = false;
        if (mWallJumping && wallJumpTickTimer > 0) mWallJumping = false;
    }

    // Apply Fall Speed Limit (if not actively wall sliding with its own speed limit and not fast sliding)
    bool isSlowWallSliding = (mWallInteractionState == PlayerWallInteractionState::SLIDING_LEFT ||
                              mWallInteractionState == PlayerWallInteractionState::SLIDING_RIGHT) && !mFastWallSlide;

    if (!isSlowWallSliding) { // If fast wall sliding or not wall sliding at all
        if (current.velocity.y < mPhysicsSettings.FallSpeedLimit) {
            // Ensure we don't counteract jump impulses immediately
            if (!mJumping && !mDoubleJumping && !mWallJumping) { // Or if (current.velocity.y < 0) to allow jump peak
                 acceleration.y += -(current.velocity.y - mPhysicsSettings.FallSpeedLimit) / timeStep;
            }
        }
    }

    // Update legacy mWallHug flags (for compatibility, e.g. animation)
    mWallHugLeft = (mWallInteractionState == PlayerWallInteractionState::SLIDING_LEFT || mWallInteractionState == PlayerWallInteractionState::CAN_JUMP_OR_SLIDE_LEFT);
    mWallHugRight = (mWallInteractionState == PlayerWallInteractionState::SLIDING_RIGHT || mWallInteractionState == PlayerWallInteractionState::CAN_JUMP_OR_SLIDE_RIGHT);

} // End of MovementUpdate