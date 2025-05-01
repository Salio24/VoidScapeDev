
#include "AudioHandler.hpp"

AudioHandler::AudioHandler() {
}

AudioHandler::~AudioHandler() {
	Mix_FreeChunk(Jump);
	Mix_FreeChunk(DoubleJump);
	Mix_FreeChunk(Slide);
	Mix_FreeChunk(WallSlide);
	Mix_FreeChunk(WindHard);
	Mix_FreeChunk(WindSoft);
	Mix_FreeChunk(BlackHoleBorn);
	Mix_FreeChunk(PortalEscape);
	Mix_FreeChunk(ConsumedByVoid);
	Mix_FreeChunk(BlackHoleIdle);
	Mix_FreeChunk(PortalIdle);
	Mix_FreeChunk(FellDown);
	Mix_FreeChunk(ButtonHover);
	Mix_FreeChunk(ButtonClick);
	Mix_FreeChunk(EscapeClick);

	Mix_FreeChunk(DoubleJumpRecharge);

	Mix_FreeMusic(IntroMusic);
	Mix_FreeMusic(LoopMusic);
	Mix_FreeMusic(LoopMenuMusic);

	Mix_FreeChunk(RunSounds[0]);
	Mix_FreeChunk(RunSounds[1]);
	Mix_FreeChunk(RunSounds[2]);
	Mix_FreeChunk(RunSounds[3]);
	Mix_FreeChunk(RunSounds[4]);
	Mix_FreeChunk(RunSounds[5]);
	Mix_FreeChunk(RunSounds[6]);
	Mix_FreeChunk(RunSounds[7]);
	Mix_FreeChunk(RunSounds[8]);
	Mix_FreeChunk(RunSounds[9]);

	Mix_FreeChunk(LandSoftSounds[0]);

	Mix_FreeChunk(LandHardSounds[0]);
}

void AudioHandler::LoadSounds() {
	Jump =           Mix_LoadWAV("assets/Sounds/Jump.wav");
	DoubleJump =     Mix_LoadWAV("assets/Sounds/DoubleJump.wav");
	Slide =          Mix_LoadWAV("assets/Sounds/Slide.wav");
	WallSlide =      Mix_LoadWAV("assets/Sounds/Wall.wav");
	WindHard =       Mix_LoadWAV("assets/Sounds/wind-blowing-sfx-12809.wav");
	WindSoft =       Mix_LoadWAV("assets/Sounds/WindSoft.wav");
	BlackHoleBorn =  Mix_LoadWAV("assets/Sounds/18_Thunder_02.wav");
	PortalEscape =   Mix_LoadWAV("assets/Sounds/13_Ice_explosion_01.wav");
	ConsumedByVoid = Mix_LoadWAV("assets/Sounds/04_Fire_explosion_04_medium.wav");
	BlackHoleIdle =  Mix_LoadWAV("assets/Sounds/BlackHoleIdle.wav");
	PortalIdle =     Mix_LoadWAV("assets/Sounds/PortalIdle.wav");
	FellDown =       Mix_LoadWAV("assets/Sounds/Died.wav");

	ButtonHover = Mix_LoadWAV("assets/Sounds/001_Hover_01.wav");
	ButtonClick = Mix_LoadWAV("assets/Sounds/013_Confirm_03.wav");
	EscapeClick = Mix_LoadWAV("assets/Sounds/029_Decline_09.wav");

	DoubleJumpRecharge = Mix_LoadWAV("assets/Sounds/092_Pause_04.wav");

	IntroMusic = Mix_LoadMUS("assets/Sounds/Anime Beat Intro.wav");
	LoopMusic =  Mix_LoadMUS("assets/Sounds/Anime Beat Loop2.wav");
	LoopMenuMusic = Mix_LoadMUS("assets/Sounds/Try and Solve This Loop.wav");


	RunSounds[0] = Mix_LoadWAV("assets/Sounds/Run01.wav");
	RunSounds[1] = Mix_LoadWAV("assets/Sounds/Run02.wav");
	RunSounds[2] = Mix_LoadWAV("assets/Sounds/Run03.wav");
	RunSounds[3] = Mix_LoadWAV("assets/Sounds/Run04.wav");
	RunSounds[4] = Mix_LoadWAV("assets/Sounds/Run05.wav");
	RunSounds[5] = Mix_LoadWAV("assets/Sounds/Run06.wav");
	RunSounds[6] = Mix_LoadWAV("assets/Sounds/Run07.wav");
	RunSounds[7] = Mix_LoadWAV("assets/Sounds/Run08.wav");
	RunSounds[8] = Mix_LoadWAV("assets/Sounds/Run09.wav");
	RunSounds[9] = Mix_LoadWAV("assets/Sounds/Run10.wav");

	LandSoftSounds[0] = Mix_LoadWAV("assets/Sounds/03_Step_grass_03.wav");

	LandHardSounds[0] = Mix_LoadWAV("assets/Sounds/LandHard.wav");
}

void AudioHandler::PlayNextStepSound() {
	if (runSoundIndex == 10) {
		runSoundIndex = 0;
	}
	Mix_PlayChannel(1, RunSounds[0], 0);
	runSoundIndex++;
}

void AudioHandler::PlayNextLandSoftSound() {
	if (landSoftSoundIndex == 5) {
		landSoftSoundIndex = 0;
	}
	Mix_PlayChannel(7, LandSoftSounds[0], 0);
	landSoftSoundIndex++;
}

void AudioHandler::PlayNextLandHardSound() {
	if (landHardSoundIndex == 1) {
		landHardSoundIndex = 0;
	}
	Mix_PlayChannel(7, LandHardSounds[0], 0);
	landHardSoundIndex++;
}

void AudioHandler::SetGlobalSFXVolume(int volume) {
	for (int i = 0; i < Mix_AllocateChannels(-1); ++i) {
		if (i != 4 && i != 10) {
			Mix_Volume(i, static_cast<int>(128 * float(volume / 10.0f)));
		}
	}
	mGlobalSFXVolumeModifier = float(volume / 10.0f);
}

void AudioHandler::SetGlobalMusicVolume(int volume) {
	Mix_VolumeMusic(static_cast<int>(mInitialMusicVolume * float(volume / 10.0f)));
}