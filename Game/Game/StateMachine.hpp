#pragma once
#include "Actor.hpp"
#include <glm/glm.hpp>
#include "MovementHandler.hpp"
#include "AnimationHandler.hpp"
#include "AudioHandler.hpp"

enum class PlayerStates {
	RUNNING,
	JUMPING,
	DOUBLE_JUMPING,
	FALLING,
	SLIDING,
	SLAMMING,
	WALLSLIDING,
	DEAD,
	HIT,
	IDLE,
	SUCKED,
	ESCAPED,
	END
};

enum class DeathCause {
	LAVA,
	FELL_DOWN,
	BLACK_HOLE,
	STUPIDITY,
	BAN_HAMMER,
	END
};


class StateMachine {
public:
	StateMachine();

	~StateMachine();

	void Update(MovementHandler& movementHandler, AnimationHandler& animationHandler, AudioHandler& audioHandler, Actor& actor, const float& deltaTime);

	void Reset();

	bool CheckPlayerStateChange();

	bool CompareToLastState(const PlayerStates& state);

	glm::vec2 mCurrentActorDrawSize{ 0.0f, 0.0f };
	glm::vec2 mCurrentActorTextureSize{ 0.0f, 0.0f };
	glm::vec2 mCurrentActorTexturePosition{ 0.0f, 0.0f };
	uint32_t mCurrentActorTextureIndex{ 0 };
	bool mActorFlipped{ false };

	DeathCause mActorDeathCause = DeathCause::END;

	PlayerStates mCurrentPlayerState = PlayerStates::IDLE;

private:

	int FallVolume{ 1 };
	bool runAnimationOneShot{ true };
	bool playingRunSound{ false };
	float FallVolumeTime{ 0.1f };
	float FallVolumeTimer{ 0.0f };


	bool deadAnimOneShot{ true };
	bool deadAnimDone{ false };
	int SuckedVolume{ 1 };
	float SuckedVolumeTime{ 0.2f };
	float SuckedVolumeTimer{ 0.0f };

	PlayerStates lastState = PlayerStates::IDLE;

	void CheckPlayerState(Actor& actor, MovementHandler& movementHandler);
};