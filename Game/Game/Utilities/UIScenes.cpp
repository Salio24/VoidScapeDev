#include "UIScenes.hpp"
#include <fstream>

UIScenes::UIScenes() {

}

UIScenes::~UIScenes() {

}
void UIScenes::LoadMainMenu() {

}


void UIScenes::LoadMainMenuBackground(std::string path, const int& tilesetOffset) {
	std::ifstream jsonStream(path);

	nlohmann::json GameLevelDataJson = nlohmann::json::parse(jsonStream);

	int levelWidth = GameLevelDataJson["width"];
	int levelHeight = GameLevelDataJson["height"];

	int blockSize = 40;
	std::vector<std::vector<int>> data(levelHeight, std::vector<int>(levelWidth));
	std::vector vec = GameLevelDataJson["layers"][0]["data"].get<std::vector<int>>();
	int offset = GameLevelDataJson["tilesets"][0]["firstgid"];
	for (int i = 0; i < levelHeight; i++) {
		for (int j = 0; j < levelWidth; j++) {
			//data[i][j] = vec[i * levelWidth + j];
			if (vec[i * levelWidth + j] != 0) {
				GameObject obj;
				obj.mSprite.mVertexData.Position = glm::vec2(j * blockSize, ((levelHeight - i) * blockSize) - blockSize);
				obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
				obj.mSprite.mVertexData.TextureIndex = tilesetOffset + vec[i * levelWidth + j] - 1;
				obj.mSprite.mVertexData.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				obj.mIsVisible = true;
				this->mBackgroundBlocks.push_back(obj);
			}
		}
	}
}

void UIScenes::BuildMainMenu() {

}

void UIScenes::BuildTitleScreen() {

}

void UIScenes::LoadMainMenuTab(const int& tilesetOffset) {
	mActiveTab = MenuTabs::MAIN;

	glm::vec4 color(0.7f, 0.7f, 0.7f, 1.0f);


	{
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 400.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 400.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Play";
		t.Scale = 0.25f;

		mButtonMap.emplace("PLAY", b);

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["PLAY"].mModelMatrix;
	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 300.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 300.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Settings";
		t.Scale = 0.25f;

		mButtonMap.emplace("SETTINGS", b);
		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["SETTINGS"].mModelMatrix;
	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 200.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 200.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Quit";
		t.Scale = 0.25f;


		mButtonMap.emplace("QUIT", b);
		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["QUIT"].mModelMatrix;
	}

	Text y;
	y.Possition = glm::vec2(960.0f, 900.0f);
	y.Color = glm::vec3(1.0f, 0.9f, 1.0f);
	y.TextField = "Void Scape";
	y.Scale = 1.0f;
	mTextToRender.push_back(y);
	mTextToRender.back().ModelMatrix = &mainMenuModelMatrix;
}

void UIScenes::LoadMainMenuSettingsTab(const int& tilesetOffset1, const int& tilesetOffset2) {
	mActiveTab = MenuTabs::SETTINGS;

	glm::vec4 color(0.7f, 0.7f, 0.7f, 1.0f);

	{
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 4), glm::vec2(960.0f, 940.0f) + glm::vec2(1920.0f, 0.0f), 30, glm::vec4(1.0f), tilesetOffset1);
		b.mHoverReactive = false;
		t.Possition = glm::vec2(960.0f, 960.0f) + glm::vec2(1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Resolution";
		t.Scale = 0.25f;

		f.Possition = glm::vec2(960.0f, 925.0f) + glm::vec2(1920.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;


		mButtonMap.emplace("RESOLUTION", b);
		mDynamicTextMap.emplace("tRESOLUTION", f);
		mDynamicTextMap["tRESOLUTION"].ModelMatrix = &mButtonMap["RESOLUTION"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["RESOLUTION"].mModelMatrix;
	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 4), glm::vec2(960.0f, 810.0f) + glm::vec2(1920.0f, 0.0f), 30, color, tilesetOffset1);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 830.0f) + glm::vec2(1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Window mode";
		t.Scale = 0.25f;

		f.Possition = glm::vec2(960.0f, 795.0f) + glm::vec2(1920.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mButtonMap.emplace("WINDOW_MODE", b);
		mDynamicTextMap.emplace("tWINDOW_MODE", f);
		mDynamicTextMap["tWINDOW_MODE"].ModelMatrix = &mButtonMap["WINDOW_MODE"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["WINDOW_MODE"].mModelMatrix;
	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 4), glm::vec2(960.0f, 680.0f) + glm::vec2(1920.0f, 0.0f), 30, glm::vec4(1.0f), tilesetOffset1);
		b.mHoverReactive = false;
		t.Possition = glm::vec2(960.0f, 700.0f) + glm::vec2(1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Music volume";
		t.Scale = 0.25f;

		f.Possition = glm::vec2(960.0f, 665.0f) + glm::vec2(1920.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mButtonMap.emplace("MUSIC_VOLUME", b);
		mDynamicTextMap.emplace("tMUSIC_VOLUME", f);
		mDynamicTextMap["tMUSIC_VOLUME"].ModelMatrix = &mButtonMap["MUSIC_VOLUME"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["MUSIC_VOLUME"].mModelMatrix;

	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 4), glm::vec2(960.0f, 550.0f) + glm::vec2(1920.0f, 0.0f), 30, glm::vec4(1.0f), tilesetOffset1);
		b.mHoverReactive = false;
		t.Possition = glm::vec2(960.0f, 570.0f) + glm::vec2(1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Sound effect volume";
		t.Scale = 0.25f;

		f.Possition = glm::vec2(960.0f, 535.0f) + glm::vec2(1920.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mButtonMap.emplace("SFX_VOLUME", b);
		mDynamicTextMap.emplace("tSFX_VOLUME", f);
		mDynamicTextMap["tSFX_VOLUME"].ModelMatrix = &mButtonMap["SFX_VOLUME"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["SFX_VOLUME"].mModelMatrix;
	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 4), glm::vec2(960.0f, 420.0f) + glm::vec2(1920.0f, 0.0f), 30, color, tilesetOffset1);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 440.0f) + glm::vec2(1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Auto retry";
		t.Scale = 0.25f;

		f.Possition = glm::vec2(960.0f, 405.0f) + glm::vec2(1920.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mButtonMap.emplace("AUTO_RETRY", b);
		mDynamicTextMap.emplace("tAUTO_RETRY", f);
		mDynamicTextMap["tAUTO_RETRY"].ModelMatrix = &mButtonMap["AUTO_RETRY"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["AUTO_RETRY"].mModelMatrix;
	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 300.0f) + glm::vec2(1920.0f, 0.0f), 30, color, tilesetOffset1);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 300.0f) + glm::vec2(1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Controls";
		t.Scale = 0.25f;


		mButtonMap.emplace("CONTROLS", b);
		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["CONTROLS"].mModelMatrix;
	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(10, 3), glm::vec2(960.0f, 140.0f) + glm::vec2(1920.0f, 0.0f), 30, color, tilesetOffset1);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 140.0f) + glm::vec2(1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Back";
		t.Scale = 0.25f;

		mButtonMap.emplace("BACK", b);
		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["BACK"].mModelMatrix;
	} {
		Button b;

		b.CreateArrowButton(glm::vec2(630.0f, 940.0f) + glm::vec2(1920.0f, 0.0f), 80, glm::vec4(1.0f), true, tilesetOffset2);

		mButtonMap.emplace("RESOLUTION_DOWN", b);
	} {
		Button b;

		b.CreateArrowButton(glm::vec2(1290.0f, 940.0f) + glm::vec2(1920.0f, 0.0f), 80, glm::vec4(1.0f), false, tilesetOffset2);

		mButtonMap.emplace("RESOLUTION_UP", b);
	} {
		Button b;

		b.CreateArrowButton(glm::vec2(630.0f, 550.0f) + glm::vec2(1920.0f, 0.0f), 80, glm::vec4(1.0f), true, tilesetOffset2);

		mButtonMap.emplace("SFX_VOLUME_DOWN", b);
	} {
		Button b;

		b.CreateArrowButton(glm::vec2(1290.0f, 550.0f) + glm::vec2(1920.0f, 0.0f), 80, glm::vec4(1.0f), false, tilesetOffset2);

		mButtonMap.emplace("SFX_VOLUME_UP", b);
	} {
		Button b;

		b.CreateArrowButton(glm::vec2(630.0f, 680.0f) + glm::vec2(1920.0f, 0.0f), 80, glm::vec4(1.0f), true, tilesetOffset2);

		mButtonMap.emplace("MUSIC_VOLUME_DOWN", b);
	} {
		Button b;

		b.CreateArrowButton(glm::vec2(1290.0f, 680.0f) + glm::vec2(1920.0f, 0.0f), 80, glm::vec4(1.0f), false, tilesetOffset2);

		mButtonMap.emplace("MUSIC_VOLUME_UP", b);
	}
}

void UIScenes::LoadMainMenuControlsTab(const int& tilesetOffset) {
	mActiveTab = MenuTabs::CONTROLS;

	glm::vec4 color(0.7f, 0.7f, 0.7f, 1.0f);

	{
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 960.0f) + glm::vec2(3840.0f, 0.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(720.0f, 960.0f) + glm::vec2(3840.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Left:";
		t.Scale = 0.25f;
		t.CenteredX = false;

		f.Possition = glm::vec2(1100.0f, 960.0f) + glm::vec2(3840.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mButtonMap.emplace("LEFT_BIND", b);
		mDynamicTextMap.emplace("tLEFT_BIND", f);
		mDynamicTextMap["tLEFT_BIND"].ModelMatrix = &mButtonMap["LEFT_BIND"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["LEFT_BIND"].mModelMatrix;
	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 860.0f) + glm::vec2(3840.0f, 0.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(720.0f, 860.0f) + glm::vec2(3840.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Right:";
		t.Scale = 0.25f;
		t.CenteredX = false;

		f.Possition = glm::vec2(1100.0f, 860.0f) + glm::vec2(3840.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mButtonMap.emplace("RIGHT_BIND", b);
		mDynamicTextMap.emplace("tRIGHT_BIND", f);
		mDynamicTextMap["tRIGHT_BIND"].ModelMatrix = &mButtonMap["RIGHT_BIND"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["RIGHT_BIND"].mModelMatrix;
	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 760.0f) + glm::vec2(3840.0f, 0.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(720.0f, 760.0f) + glm::vec2(3840.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Jump:";
		t.Scale = 0.25f;
		t.CenteredX = false;

		f.Possition = glm::vec2(1100.0f, 760.0f) + glm::vec2(3840.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mButtonMap.emplace("JUMP_BIND", b);
		mDynamicTextMap.emplace("tJUMP_BIND", f);
		mDynamicTextMap["tJUMP_BIND"].ModelMatrix = &mButtonMap["JUMP_BIND"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["JUMP_BIND"].mModelMatrix;
	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 660.0f) + glm::vec2(3840.0f, 0.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(720.0f, 660.0f) + glm::vec2(3840.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Slide/Slam:";
		t.Scale = 0.25f;
		t.CenteredX = false;

		f.Possition = glm::vec2(1100.0f, 660.0f) + glm::vec2(3840.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mButtonMap.emplace("DUCK_BIND", b);
		mDynamicTextMap.emplace("tDUCK_BIND", f);
		mDynamicTextMap["tDUCK_BIND"].ModelMatrix = &mButtonMap["DUCK_BIND"].mModelMatrix;

		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["DUCK_BIND"].mModelMatrix;
	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(10, 3), glm::vec2(960.0f, 140.0f) + glm::vec2(3840.0f, 0.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 140.0f) + glm::vec2(3840.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Back";
		t.Scale = 0.25f;

		mButtonMap.emplace("BACK_CONTROLS_TAB", b);
		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["BACK_CONTROLS_TAB"].mModelMatrix;
	}
}

void UIScenes::LoadMainMenuLevelsTab(const int& tilesetOffset, bool debugMode) {
	mActiveTab = MenuTabs::LEVELS;

	glm::vec4 color(0.7f, 0.7f, 0.7f, 1.0f);

	{
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(25, 3), glm::vec2(960.0f, 960.0f) + glm::vec2(-1920.0f, 0.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(620.0f, 960.0f) + glm::vec2(-1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Level 1";
		t.Scale = 0.25f;
		t.CenteredX = false;

		f.Possition = glm::vec2(1300.0f, 960.0f) + glm::vec2(-1920.0f, 0.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;
		f.CenteredX = false;
		f.RightSided = true;

		mButtonMap.emplace("LEVEL_1", b);
		mDynamicTextMap.emplace("tLEVEL_1", f);
		mDynamicTextMap["tLEVEL_1"].ModelMatrix = &mButtonMap["LEVEL_1"].mModelMatrix;


		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["LEVEL_1"].mModelMatrix;
	}
	if (debugMode) {
		{
			Button b;
			Text t;

			b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 830.0f) + glm::vec2(-1920.0f, 0.0f), 30, color, tilesetOffset);
			b.mHoverReactive = true;
			t.Possition = glm::vec2(960.0f, 830.0f) + glm::vec2(-1920.0f, 0.0f);
			t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
			t.TextField = "TEST LEVEL";
			t.Scale = 0.25f;

			mButtonMap.emplace("TEST_LEVEL", b);
			mTextToRender.push_back(t);
			mTextToRender.back().ModelMatrix = &mButtonMap["TEST_LEVEL"].mModelMatrix;
		}
	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(10, 3), glm::vec2(960.0f, 140.0f) + glm::vec2(-1920.0f, 0.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 140.0f) + glm::vec2(-1920.0f, 0.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Back";
		t.Scale = 0.25f;

		mButtonMap.emplace("BACK_LEVEL_TAB", b);
		mTextToRender.push_back(t);
		mTextToRender.back().ModelMatrix = &mButtonMap["BACK_LEVEL_TAB"].mModelMatrix;
	}
}

void UIScenes::LoadTitleScreen(const int& tilesetOffset) {
	glm::vec4 color(0.7f, 0.7f, 0.7f, 1.0f);

	{
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 580.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 580.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Sample Text";
		t.Scale = 0.25f;

		mInGameTitleButtonMap.emplace("RETRY", b);
		mInGameTitleDynamicTextMap.emplace("tRETRY", t);
		mInGameTitleDynamicTextMap["tRETRY"].ModelMatrix = &mInGameTitleButtonMap["RETRY"].mModelMatrix;
	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 4), glm::vec2(960.0f, 460.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 480.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Auto Retry";
		t.Scale = 0.25f;
		
		f.Possition = glm::vec2(960.0f, 440.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mInGameTitleButtonMap.emplace("AUTO_RETRY", b);
		mInGameTitleDynamicTextMap.emplace("tAUTO_RETRY", f);
		mInGameTitleDynamicTextMap["tAUTO_RETRY"].ModelMatrix = &mInGameTitleButtonMap["AUTO_RETRY"].mModelMatrix;

		mInGameTitleTextToRender.push_back(t);
		mInGameTitleTextToRender.back().ModelMatrix = &mInGameTitleButtonMap["AUTO_RETRY"].mModelMatrix;

	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 200.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 200.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Back to main menu";
		t.Scale = 0.25f;

		mInGameTitleButtonMap.emplace("BACK_TO_MENU", b);

		mInGameTitleTextToRender.push_back(t);
		mInGameTitleTextToRender.back().ModelMatrix = &mInGameTitleButtonMap["BACK_TO_MENU"].mModelMatrix;

	}

	{
		Text t;

		t.Possition = glm::vec2(960.0f, 900.0f);
		t.Color = glm::vec3(1.0f);
		t.TextField = "Sample Text";
		t.Scale = 1.0f;

		mInGameTitleDynamicTextMap.emplace("tTITLE", t);
		mInGameTitleDynamicTextMap["tTITLE"].ModelMatrix = &globalUIModelMatrix;
	} {
		Text t;

		t.Possition = glm::vec2(960.0f, 770.0f);
		t.Color = glm::vec3(1.0f);
		t.TextField = "Sample Text";
		t.Scale = 0.25f;

		mInGameTitleDynamicTextMap.emplace("tDEATH_CAUSE", t);
		mInGameTitleDynamicTextMap["tDEATH_CAUSE"].ModelMatrix = &globalUIModelMatrix;
	} {
		Text t;

		t.Possition = glm::vec2(960.0f, 825.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Sample Text";
		t.Scale = 0.25f;

		mInGameTitleDynamicTextMap.emplace("tTIME", t);
		mInGameTitleDynamicTextMap["tTIME"].ModelMatrix = &globalUIModelMatrix;
	} {
		Text t;

		t.Possition = glm::vec2(960.0f, 770.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Sample Text";
		t.Scale = 0.25f;

		mInGameTitleDynamicTextMap.emplace("tBEST_TIME", t);
		mInGameTitleDynamicTextMap["tBEST_TIME"].ModelMatrix = &globalUIModelMatrix;
	} {
		Text t;

		t.Possition = glm::vec2(960.0f, 700.0f);
		t.Color = glm::vec3(0.8671875f, 0.72265625f, 0.0f);
		t.TextField = "Sample Text";
		t.Scale = 0.5f;

		mInGameTitleDynamicTextMap.emplace("tNEW_BEST_TIME", t);
		mInGameTitleDynamicTextMap["tNEW_BEST_TIME"].ModelMatrix = &globalUIModelMatrix;
	}
}

void UIScenes::LoadPauseMenu(const int& tilesetOffset) {
	glm::vec4 color(0.7f, 0.7f, 0.7f, 1.0f);


	{
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 580.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 580.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Continue";
		t.Scale = 0.25f;

		mInGamePauseButtonMap.emplace("CONTINUE", b);

		mInGamePauseTextToRender.push_back(t);
		mInGamePauseTextToRender.back().ModelMatrix = &mInGamePauseButtonMap["CONTINUE"].mModelMatrix;
	} {
		Button b;
		Text t;
		Text f;

		b.CreateBoxButtonCentered(glm::ivec2(20, 4), glm::vec2(960.0f, 460.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 480.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Auto Retry";
		t.Scale = 0.25f;

		f.Possition = glm::vec2(960.0f, 440.0f);
		f.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		f.TextField = "Sample Text";
		f.Scale = 0.25f;

		mInGamePauseButtonMap.emplace("AUTO_RETRY", b);
		mInGamePauseDynamicTextMap.emplace("tAUTO_RETRY", f);
		mInGamePauseDynamicTextMap["tAUTO_RETRY"].ModelMatrix = &mInGamePauseButtonMap["AUTO_RETRY"].mModelMatrix;


		mInGamePauseTextToRender.push_back(t);
		mInGamePauseTextToRender.back().ModelMatrix = &mInGamePauseButtonMap["AUTO_RETRY"].mModelMatrix;
	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 340.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 340.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Restart";
		t.Scale = 0.25f;

		mInGamePauseButtonMap.emplace("RESTART", b);

		mInGamePauseTextToRender.push_back(t);
		mInGamePauseTextToRender.back().ModelMatrix = &mInGamePauseButtonMap["RESTART"].mModelMatrix;
	} {
		Button b;
		Text t;

		b.CreateBoxButtonCentered(glm::ivec2(20, 3), glm::vec2(960.0f, 200.0f), 30, color, tilesetOffset);
		b.mHoverReactive = true;
		t.Possition = glm::vec2(960.0f, 200.0f);
		t.Color = glm::vec3(0.80859375f, 0.80078125f, 0.81640625f);
		t.TextField = "Back to main menu";
		t.Scale = 0.25f;

		mInGamePauseButtonMap.emplace("BACK_TO_MENU", b);

		mInGamePauseTextToRender.push_back(t);
		mInGamePauseTextToRender.back().ModelMatrix = &mInGamePauseButtonMap["BACK_TO_MENU"].mModelMatrix;
	}

	Text y;

	y.Possition = glm::vec2(960.0f, 900.0f);
	y.Color = glm::vec3(1.0f, 0.0f, 0.14f);
	y.TextField = "Paused";
	y.Scale = 1.0f;
	mInGamePauseTextToRender.push_back(y);
	mInGamePauseTextToRender.back().ModelMatrix = &globalUIModelMatrix;
}

void UIScenes::TranslateMenuSmooth(const glm::vec2& translateGoal, const float& deltaTime, const float& speed) {
	if (animSpeed == 0.0f) {
		animSpeed = speed;
	}

	glm::vec2 translateAmmount = glm::normalize(translateGoal) * animSpeed * deltaTime;
	animSpeed *= 1.0f + (deltaTime * 4.0f);
	translated += translateAmmount;

	if (std::abs(translated.x) > std::abs(translateGoal.x) || std::abs(translated.y) > std::abs(translateGoal.y)) {
		translateAmmount -= translated - translateGoal;
		translated -= translated - translateGoal;
		translated = glm::vec2(0.0f, 0.0f);
		animSpeed = 0.0f;
		mNextTabLoaded = true;
	}

	translateOffset += translateAmmount;
	for (auto& pair : mButtonMap) {
		mButtonMap[pair.first].Translate(translateAmmount);
	}
}

void UIScenes::TranslateMenuInstant(const glm::vec2 translateGoal) {
	translateOffset += translateGoal;
	for (auto& pair : mButtonMap) {
		mButtonMap[pair.first].Translate(translateGoal);
	}
}

void UIScenes::FillBlockVector() {
	mMainMenuBlocks.clear();
	mMainMenuBlocks.insert(mMainMenuBlocks.end(), mBackgroundBlocks.begin(), mBackgroundBlocks.end());
}

void UIScenes::Update(const glm::mat4& matrix) {
	globalUIModelMatrix = matrix;
	mainMenuModelMatrix = glm::translate(matrix, glm::vec3(translateOffset.x, translateOffset.y, 0.0f));
}

Button::Button() {

}

Button::~Button() {

}

void Button::CreateBoxButton(glm::ivec2 size, const glm::vec2& position, const int& blockSize, const glm::vec4& color, const int& tilesetOffset) {
	if (!mCreated) {
		if (size.x < 2 || size.y < 2) {
			std::cerr << "Button size must be at least 2x2, wrong size" << std::endl;
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 1);

		std::vector<int> upperRow;
		upperRow.push_back(5);
		upperRow.push_back(6);
		std::vector<int> lowerRow;
		lowerRow.push_back(29);
		lowerRow.push_back(30);
		std::vector<int> leftColumn;
		leftColumn.push_back(12);
		leftColumn.push_back(20);
		std::vector<int> rightColumn;
		rightColumn.push_back(15);
		rightColumn.push_back(23);

		int UL_Corner = 4;
		int UR_Corner = 7;
		int LL_Corner = 28;
		int LR_Corner = 31;

		{
			GameObject obj;
			obj.mSprite.mVertexData.Position = glm::vec2(position.x, position.y);
			obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
			obj.mSprite.mVertexData.TextureIndex = tilesetOffset + LL_Corner;
			obj.mSprite.mVertexData.Color = color;
			mButtonBlocks.push_back(obj);
		} {
			GameObject obj;
			obj.mSprite.mVertexData.Position = glm::vec2(position.x, position.y + ((size.y - 1) * blockSize));
			obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
			obj.mSprite.mVertexData.TextureIndex = tilesetOffset + UL_Corner;
			obj.mSprite.mVertexData.Color = color;
			mButtonBlocks.push_back(obj);
		} {
			GameObject obj;
			obj.mSprite.mVertexData.Position = glm::vec2(position.x + ((size.x - 1) * blockSize), position.y);
			obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
			obj.mSprite.mVertexData.TextureIndex = tilesetOffset + LR_Corner;
			obj.mSprite.mVertexData.Color = color;
			mButtonBlocks.push_back(obj);
		} {
			GameObject obj;
			obj.mSprite.mVertexData.Position = glm::vec2(position.x + ((size.x - 1) * blockSize), position.y + ((size.y - 1) * blockSize));
			obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
			obj.mSprite.mVertexData.TextureIndex = tilesetOffset + UR_Corner;
			obj.mSprite.mVertexData.Color = color;
			mButtonBlocks.push_back(obj);
		}

		if (size.x > 2 || size.y > 2) {
			for (int i = 0; i < size.x; i++) {
				for (int j = 0; j < size.y; j++) {
					GameObject obj;
					if (j != 0 && j != size.y - 1) {
						if (i == 0) {
							obj.mSprite.mVertexData.Position = glm::vec2(position.x, position.y + (j * blockSize));
							obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
							obj.mSprite.mVertexData.TextureIndex = tilesetOffset + leftColumn[dist(gen)];
							obj.mSprite.mVertexData.Color = color;
						}
						else if (i == size.x - 1) {
							obj.mSprite.mVertexData.Position = glm::vec2(position.x + ((size.x - 1) * blockSize), position.y + (j * blockSize));
							obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
							obj.mSprite.mVertexData.TextureIndex = tilesetOffset + rightColumn[dist(gen)];
							obj.mSprite.mVertexData.Color = color;
						}
					}
					else if (i != 0 && i != size.x - 1) {
						if (j == 0) {
							obj.mSprite.mVertexData.Position = glm::vec2(position.x + (i * blockSize), position.y);
							obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
							obj.mSprite.mVertexData.TextureIndex = tilesetOffset + lowerRow[dist(gen)];
							obj.mSprite.mVertexData.Color = color;
						}
						else if (j == size.y - 1) {
							obj.mSprite.mVertexData.Position = glm::vec2(position.x + (i * blockSize), position.y + ((size.y - 1) * blockSize));
							obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
							obj.mSprite.mVertexData.TextureIndex = tilesetOffset + upperRow[dist(gen)];
							obj.mSprite.mVertexData.Color = color;
						}
					}
					mButtonBlocks.push_back(obj);
				}
			}
		}


		mSize.x = size.x * blockSize;
		mSize.y = size.y * blockSize;
		mCenterPoint.x = position.x + mSize.x / 2.0f;
		mCenterPoint.y = position.y + mSize.y / 2.0f;
		mPosition = position;
		mCreated = true;
	}
	else {
		std::cerr << "ACHTUNG: Button already created" << std::endl;
	}
}

void Button::CreateBoxButtonCentered(glm::ivec2 size, const glm::vec2& position, const int& blockSize, const glm::vec4& color, const int& tilesetOffset) {
	CreateBoxButton(size, glm::vec2(position.x - (((float)size.x / 2.0f) * blockSize), position.y - (((float)size.y / 2.0f) * blockSize)), blockSize, color, tilesetOffset);
}

void Button::CreateArrowButton(const glm::vec2& position, const int& blockSize, const glm::vec4& color, const bool& direction, const int& tilesetOffset, const bool& centered) {
	glm::vec2 convertedPos;
	int index;

	if (!mCreated) {
		if (centered) {
			convertedPos.x = position.x - ((float)blockSize / 2.0f);
			convertedPos.y = position.y - ((float)blockSize / 2.0f);
		}
		else {
			convertedPos = position;
		}
		if (direction) {
			index = 0;

			mPosition = glm::vec2(convertedPos.x + ((float)blockSize / 8.0f), convertedPos.y + ((float)blockSize / 8.0f));
			mSize = glm::vec2((float)blockSize / (16.0f / 9.0f), (float)blockSize / (16.0f / 12.0f));
		}
		else {
			index = 1;

			mPosition = glm::vec2(convertedPos.x + ((float)blockSize / 4.0f), convertedPos.y + ((float)blockSize / 8.0f));
			mSize = glm::vec2((float)blockSize / (16.0f / 9.0f), (float)blockSize / (16.0f / 12.0f));
		}

		GameObject obj;
		obj.mSprite.mVertexData.Position = glm::vec2(convertedPos.x, convertedPos.y);
		obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
		obj.mSprite.mVertexData.TextureIndex = tilesetOffset + index;
		obj.mSprite.mVertexData.Color = color;

		mCreated = true;

		mCenterPoint.x = convertedPos.x + blockSize / 2.0f;
		mCenterPoint.y = convertedPos.y + blockSize / 2.0f;

		mButtonBlocks.push_back(obj);
		arrowButton = true;
	}
	else {
		std::cerr << "ACHTUNG: Button already created" << std::endl;
	}
}

bool Button::GetPressState() {
	bool buffer = pressState;
	pressState = false;
	return buffer;
}

bool Button::GetHoverState() {
	return hoverState;
}

void Button::SetPressState(const bool &state) {
	pressState = state;
}

void Button::SetHoverState(const bool& state) {
	hoverState = state;
}

void Button::ChangeColor(const glm::vec3& color) {
	for (int i = 0; i < mButtonBlocks.size(); i++) {
		mButtonBlocks[i].mSprite.mVertexData.Color = glm::vec4(color, 1.0f);
	}
}

bool Button::CheckHoverStateChange() {
	bool result = (hoverState != oldHoverState);
	oldHoverState = hoverState;
	return result;
}

void Button::Render(BatchRenderer* renderer, const glm::mat4& projectionMatrix, const glm::mat4& mUIModelMatrix, GLuint textureArray) {

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

	if (!arrowButton) {
		renderer->BeginBatch(projectionMatrix, &mUIModelMatrix);

		renderer->DrawInBatch(mTriggerPos, mTriggerSize, glm::vec4((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), 0.8f));

		renderer->EndBatch();
		renderer->Flush();
	}

	renderer->BeginBatch(projectionMatrix, &mModelMatrix);

	for(int i = 0; i < mButtonBlocks.size(); i++) {
		renderer->DrawInBatch(mButtonBlocks[i].mSprite.mVertexData.Position, mButtonBlocks[i].mSprite.mVertexData.Size, static_cast<uint32_t>(mButtonBlocks[i].mSprite.mVertexData.TextureIndex), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), 0.0f, 1.0f, false, mButtonBlocks[i].mSprite.mVertexData.Color);
	}

	renderer->EndBatch();
	renderer->Flush();
}

void Button::Update(const glm::mat4& modelMatrix, Mix_Chunk* hoverSound, Mix_Chunk* clickSound) {
	if (pressState && mHoverReactive) {
		if (clickSoundOneShot) {
			Mix_PlayChannel(15, clickSound, 0);
			clickSoundOneShot = false;
		}
	}
	else {
		clickSoundOneShot = true;
	}

	if ((hoverState || mInteracting) && mHoverReactive) {
		//need to change trigger size here

		if (hoverSoundOneShot) {
			Mix_PlayChannel(16, hoverSound, 0);
			hoverSoundOneShot = false;
		}

		float scaleFactor = 1.16f;
		glm::vec2 pt = glm::vec2(mCenterPoint.x - (mCenterPoint.x / scaleFactor), mCenterPoint.y - (mCenterPoint.y / scaleFactor));
		mModelMatrix = glm::translate(modelMatrix, glm::vec3(mTranslateOffset.x, mTranslateOffset.y, 0.0f));
		mModelMatrix = glm::scale(mModelMatrix, glm::vec3(scaleFactor, scaleFactor, 1.0f));
		mModelMatrix = glm::translate(mModelMatrix, glm::vec3(-pt.x, -pt.y, 0.0f));


		mTriggerSize = glm::vec2(mSize.x * (scaleFactor - 0.02f), mSize.y * (scaleFactor - 0.1f));

	}
	else {
		if (!hoverSoundOneShot) {
			hoverSoundOneShot = true;
		}

		mModelMatrix = glm::translate(modelMatrix, glm::vec3(mTranslateOffset.x, mTranslateOffset.y, 0.0f));

		mTriggerSize = glm::vec2(mSize.x * 0.98f, mSize.y * 0.9f);
		
	}
	mTriggerPos = mCenterPoint - mTriggerSize / 2.0f + mTranslateOffset;
}

void Button::Translate(const glm::vec2& translateAmmount) {
	mTranslateOffset += translateAmmount;
	mPosition += translateAmmount;
}


