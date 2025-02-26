#pragma once
#include <SDL3/SDL.h>
#include "Graphics/Camera.hpp"
#include <glad/glad.h>
#include "Utilities/Input.hpp"
#include <glm/glm.hpp>
#include "Actors/Actor.hpp"
#include "Graphics/Renderers/BatchRenderer.hpp"
#include "Utilities/HelperStructs.hpp"
#include "Physics/MovementHandler.hpp"
#include <vector>
#include <chrono>
#include "Graphics/Texture.hpp"
#include "Graphics/AnimationHandler.hpp"
#include "Utilities/AudioHandler.hpp"
#include "Actors/BlackHole.hpp"
#include "InternalLogic/StateMachine.hpp"
#include "Actors/EscapePortal.hpp"
#include "Graphics/Shaders/ShaderProgram.hpp"
#include "Graphics/PipelineProgram.hpp"
#include "Graphics/Renderers/TextRenderer.hpp"
#include "InternalLogic/Scenes/SceneManager.hpp"
#include "Graphics/Renderers/BackgroundRenderer.hpp"
#include "Utilities/JsonFileManager.hpp"
#include "glm/gtx/string_cast.hpp"
#include <random>
#include <glm/gtx/exterior_product.hpp>
#include "Physics/PhysicsComponent.hpp"

//struct State
//{
//	float x;      // position
//	float v;      // velocity
//};
//
//struct Derivative
//{
//	float dx;      // dx/dt = velocity
//	float dv;      // dv/dt = acceleration
//};

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

	PhysicsComponent mPhysicsComponent;

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
	double dtD{ 0.0f };

	float textSizeMultiplier    { 800.0f };

	float startMessageTime { 2.0f };
	float startMessageTimer{ 0.0f };

	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> TimePoint1;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> TimePoint2;

	SDL_Window* mWindow     { nullptr };
	SDL_GLContext mGlContext{ nullptr };
};