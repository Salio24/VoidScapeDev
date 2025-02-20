#pragma once
#include <string>
#include "HelperStructs.hpp"
#include "..//Renderers/BatchRenderer.hpp"
#include "../Actors/GameObject.hpp"
#include <SDL3/SDL_mixer.h>
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <unordered_map>

enum class MenuTabs {
	MAIN,
	SETTINGS,
	LEVELS,
	CONTROLS,
	END
};

enum class MenuMoveDirection {
	LEFT,
	RIGHT,
	UP,
	DOWN,
	STATIC,
	END

};

class Button {
public:
	Button();

	~Button();

	void CreateBoxButton(glm::ivec2 size, const glm::vec2& position, const int& blockSize, const glm::vec4& color, const int& tilesetOffset);
	
	void CreateBoxButtonCentered(glm::ivec2 size, const glm::vec2& position, const int& blockSize, const glm::vec4& color, const int& tilesetOffset);
	
	void CreateArrowButton(const glm::vec2& position, const int& blockSize, const glm::vec4& color, const bool& direction, const int& tilesetOffset, const bool& centered = true);

	bool GetPressState();
	
	bool GetHoverState();
	
	void SetPressState(const bool& state);
	
	void SetHoverState(const bool& stata);

	bool CheckHoverStateChange();

	void ChangeColor(const glm::vec3& color);

	void Render(BatchRenderer* renderer, const glm::mat4& projectionMatrix, const glm::mat4& mUIModelMatrix, GLuint textureArray);

	void Update(const glm::mat4& modelMatrix, Mix_Chunk* hoverSound, Mix_Chunk* clickSound);

	void Translate(const glm::vec2& translateAmmount);
	
	std::vector<GameObject> mButtonBlocks;
	
	glm::vec2 mPosition{ 0.0f, 0.0f };

	glm::vec2 mSize{ 0.0f, 0.0f };

	glm::vec2 mTriggerPos{ 0.0f, 0.0f };

	glm::vec2 mTriggerSize{ 0.0f, 0.0f };

	glm::vec2 mCenterPoint{ 0.0f, 0.0f };
	
	bool mCreated{ false };

	bool mHoverReactive{ true };

	bool mInteracting{ false };

	glm::mat4 mModelMatrix{ 1.0f };

	glm::vec2 mTranslateOffset{ 0.0f };

private:
	bool oldPressState{ false };
	bool pressState{ false };

	bool oldHoverState{ false };
	bool hoverState{ false };

	bool arrowButton{ false };

	bool hoverSoundOneShot{ true };
	bool clickSoundOneShot{ true };
};

class UIScenes {
public:

	UIScenes();
	
	~UIScenes();
	
	void LoadMainMenu();
	
	void LoadMainMenuBackground(std::string path, const int& tilesetOffset);
	
	void BuildMainMenu();
	
	void BuildTitleScreen();

	void LoadMainMenuTab(const int& tilesetOffset);

	void LoadMainMenuSettingsTab(const int& tilesetOffset1, const int& tilesetOffset2);

	void LoadMainMenuControlsTab(const int& tilesetOffset);

	void LoadMainMenuLevelsTab(const int& tilesetOffset, bool debugMode);

	void LoadTitleScreen(const int& tilesetOffset);

	void LoadPauseMenu(const int& tilesetOffset);

	void TranslateMenuSmooth(const glm::vec2& translateGoal, const float& deltaTime, const float& speed);

	void TranslateMenuInstant(const glm::vec2 translateGoal);

	void FillBlockVector();

	void Update(const glm::mat4& matrix);

	MenuTabs mActiveTab{ MenuTabs::MAIN };
	MenuTabs mNextTab{ MenuTabs::MAIN };
	MenuMoveDirection mMenuMoveDirection{ MenuMoveDirection::STATIC };

	bool mNextTabLoaded{ true };

	bool mTitleScreenActive{ false };

	std::vector<GameObject> mMainMenuBlocks;
	
	std::vector<GameObject> mBackgroundBlocks;





	std::unordered_map<std::string, Button> mButtonMap;
	std::unordered_map<std::string, Text> mDynamicTextMap;
	std::vector<Text> mTextToRender;

	std::unordered_map<std::string, Button> mInGameTitleButtonMap;
	std::unordered_map<std::string, Text> mInGameTitleDynamicTextMap;
	std::vector<Text> mInGameTitleTextToRender;

	std::unordered_map<std::string, Button> mInGamePauseButtonMap;
	std::unordered_map<std::string, Text> mInGamePauseDynamicTextMap;
	std::vector<Text> mInGamePauseTextToRender;
	
	std::vector<Button*> mAllButtons;

	float animSpeed{ 0.0f };
private:
	glm::vec2 translated{ 0.0f };

	glm::vec2 translateOffset{ 0.0f };

	glm::mat4 mainMenuModelMatrix{ 1.0f };

	glm::mat4 globalUIModelMatrix{ 1.0f };

};   
