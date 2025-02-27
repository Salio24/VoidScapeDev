#pragma once
#include <SDL3/SDL_mixer.h>
#include <array>
#include <iostream>

class AudioHandler {
public:
	AudioHandler();

	~AudioHandler();

	void LoadSounds();

	void PlayNextStepSound();

	void PlayNextLandSoftSound();

	void PlayNextLandHardSound();

	void SetGlobalSFXVolume(int volume);

	void SetGlobalMusicVolume(int volume);

	Mix_Chunk* Jump = NULL;
	Mix_Chunk* DoubleJump = NULL;
	Mix_Chunk* Slide = NULL;
	Mix_Chunk* WindHard = NULL;
	Mix_Chunk* WindSoft = NULL;
	Mix_Chunk* WallSlide = NULL;
	Mix_Chunk* BlackHoleBorn = NULL;
	Mix_Chunk* PortalEscape = NULL;
	Mix_Chunk* ConsumedByVoid = NULL;
	Mix_Chunk* BlackHoleIdle = NULL;
	Mix_Chunk* PortalIdle = NULL;
	Mix_Chunk* FellDown = NULL;
	Mix_Chunk* ButtonHover = NULL;
	Mix_Chunk* ButtonClick = NULL;
	Mix_Chunk* EscapeClick = NULL;
	Mix_Chunk* DoubleJumpRecharge = NULL;

	std::array<Mix_Chunk*, 10> RunSounds;
	std::array<Mix_Chunk*, 1> LandSoftSounds;
	std::array<Mix_Chunk*, 1> LandHardSounds;

	Mix_Music* IntroMusic = NULL;
	Mix_Music* LoopMusic = NULL;
	Mix_Music* LoopMenuMusic = NULL;

	float mGlobalSFXVolumeModifier{ 1.0f };

	int mInitialMusicVolume{ 128 };

	float mMusicFadeoutSpeed{ 20.0f };

	float mMusicFadeoutTime{ 0.05f };
	float mMusicFadeoutTimer{ 0.0f };

private:
	int runSoundIndex{ 0 };
	int landSoftSoundIndex{ 0 };
	int landHardSoundIndex{ 0 };
};