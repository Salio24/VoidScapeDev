#pragma once
#include <SDL3/SDL.h>
#include "Camera.hpp"
#include <glad/glad.h>
#include "Input.hpp"
#include <glm/glm.hpp>
#include "Actor.hpp"
#include "BatchRenderer.hpp"
#include "HelperStructs.hpp"
#include "MovementHandler.hpp"
#include <vector>
#include <chrono>
#include "Texture.hpp"
#include "AnimationHandler.hpp"
#include "AudioHandler.hpp"
#include "BlackHole.hpp"
#include "StateMachine.hpp"
#include "EscapePortal.hpp"
#include "ShaderProgram.hpp"
#include "PipelineProgram.hpp"
#include "TextRenderer.hpp"
#include "SceneManager.hpp"
#include "UIScenes.hpp"
#include "BackgroundRenderer.hpp"
#include "JsonFileManager.hpp"
#include "glm/gtx/string_cast.hpp"
#include <random>
#include <glm/gtx/exterior_product.hpp>

struct State
{
	float x;      // position
	float v;      // velocity
};

struct Derivative
{
	float dx;      // dx/dt = velocity
	float dv;      // dv/dt = acceleration
};

class App {
public:
	static App& getInstance();

	void PostStartUp();

	void LoadGame(const bool retry = false);

	void MainLoop();

	void ReloadConfig(); // debug func

	GLuint mGraphicsPipelineShaderProgram{ 0 };

	InputManager mInputManager;

	BatchRenderer mBatchRenderer;

	Actor mActor;

	MovementHandler mMovementHandler;

	AudioHandler mAudioHandler;

	EscapePortal mEscapePortal;

	TextureHandler mTextureHandler;

	AnimationHandler mAnimationHandler;

	BlackHole mBlackHole;

	StateMachine mStateMachine;

	Camera mCamera;

	ShaderProgram mGeneralShaderProgram;
	ShaderProgram mTextShaderProgram;
	ShaderProgram mBackgroundShaderProgram;
	ShaderProgram mBackgroundFramebufferShaderProgram; 

	PipelineProgram mPipelineProgram;

	SceneManager mSceneManager;

	BackgroundRenderer  mBackgroundRenderer;

	TextRenderer mTextRenderer;

	JsonManager mJsonManager;

	bool mQuit{ false };
	bool mPause{ false };

	Settings mSettings;

	SaveData mSaveData;

	Config mConfig;

	int mWindowWidth{ 1920 };
	int mWindowHeight{ 1080 };

	std::vector<std::string> mWindowModes;

	std::vector<glm::ivec2> mResolutions;

	bool testReset{ false };

	float testSpeeed{ 0.0f };

private: 
	App();

	~App();

	App(const App&) = delete;

	App& operator=(const App&) = delete;

	void StartUp();

	void ShutDown();

	void Update();

	void UpdatePlayground(float deltaTime);

	static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar* message, const void* param);

	bool gameStarted{ false };

	float deltaTime      { 0.0f };

	float textSizeMultiplier    { 800.0f };

	float startMessageTime { 2.0f };
	float startMessageTimer{ 0.0f };

	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> TimePoint1;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> TimePoint2;

	SDL_Window* mWindow     { nullptr };
	SDL_GLContext mGlContext{ nullptr };

	double accumulator = 0.0;

	State previous;
	State current;

	double t = 0.0;
	double dt = 1.0f / 10.0f;

	Derivative evaluate(const State& initial,
		double t,
		float dt,
		const Derivative& d);

	float acceleration(const State& state, double t);

	void integrate(State& state,
		double t,
		float dt);
};