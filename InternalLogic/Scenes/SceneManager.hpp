#pragma once
#include <string>
#include "../../Actors/GameObject.hpp"
#include "UIScenes.hpp"
#include "LevelScene.hpp"
#include <SDL3/SDL_mixer.h>
#include "../../Utilities/Input.hpp"
#include "../../Graphics/Renderers/BackgroundRenderer.hpp"
#include "../../Utilities/AudioHandler.hpp"
#include "../StateMachine.hpp"
#include "../../Graphics/Renderers/TextRenderer.hpp"
#include "../../Utilities/JsonFileManager.hpp"

class SceneManager {
public:
	SceneManager();

	~SceneManager();

	void UpdateUIMenu(int tilesetOffset, float deltaTime, int& windowWidth, int& windowHeight, bool& quit, std::vector<std::string>& windowModes, std::vector<glm::ivec2>& resolutions, SDL_Window* window, bool& restart, bool& reload, glm::vec2& spawn);

	void UpdateUIInGame(bool& restart, bool& restartMode, const bool actorDead, const bool actorEscaped, bool& pause, const bool gameStarted, const glm::mat4& projectionMatrix, const glm::mat4& UImodelMatrix, float deltaTime, DeathCause actorDeathCause, ShaderProgram* textShader, GLuint pipelineID, int fps, GLuint arrayTexture);

	void ResetTimer();

	void StartLevelTimer();

	void Init(InputManager* INinputManager, BackgroundRenderer* INbackgroundRenderer, AudioHandler* INaudioHandler, Settings* INsettings, SaveData* INsaveData, BatchRenderer* INbatchRenderer, TextRenderer* INtextRenderer);

	void LoadMainMenu(const int& tilesetOffset);

	void ReloadCurrentLevel(const int tilesetOffset, Mix_Music* introMusic, glm::vec2& spawn);

	void LoadLevel(Levels level, const int tilesetOffset, Mix_Music* introMusic, bool& restart, bool& reload, glm::vec2& spawn);

	bool mLevelActive{ false };

	bool mMainMenuActive{ false };

	int offset;

	int levelWidth;
	int levelHeight;

	UIScenes mUIScenes;

	LevelScene mLevelScene;

	Levels mCurrentLevel{ Levels::END };

	std::vector<GameObject> mBlocks;

	std::vector<GameObject>* mCurrentBlocks;

	std::vector<std::vector<std::vector<int>>> layoutData;

	bool mNewBestTimeMessageOneShot{ false };

	glm::vec2 mCurtainSize{ 0.0f, 1080.0f };
private:
	SaveData* saveData;
	Settings* settings;
	InputManager* inputManager;
	AudioHandler* audioHandler;
	TextRenderer* textRenderer;
	BatchRenderer* batchRenderer;
	BackgroundRenderer* backgroundRenderer;

	float titleScreenAlpha{ 0.0f };
	float titleScreenAlphaTime{ 0.01f };
	float titleScreenAlphaTimer{ 0.0f };
	bool titleScreenDarkened{ false };

	float titleScreenMusicVolumeTime{ 0.05f };
	float titleScreenMusicVolumeTimer{ 0.0f };
	int   titleScreenMusicVolume{ 128 };

	int levelTime_minutes{ 0 };
	int levelTime_seconds{ 0 };
	int levelTime_centiseconds{ 0 };

	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> levelTimerTotal;
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> pauseTimer;

	long long levelTimeTotal{ 0 };
	long long levelTime{ 0 };
	long long pauseTime{ 0 };

	bool deathMessageOneShot{ true };

	std::string deathMessageText;
	glm::vec4 deathMessageColor = glm::vec4(1.0f);
};