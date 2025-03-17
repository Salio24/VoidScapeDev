#include "SceneManager.hpp" 


SceneManager::SceneManager() {

}

SceneManager::~SceneManager() {

}

void SceneManager::UpdateUIMenu(int tilesetOffset, float deltaTime, int& windowWidth, int& windowHeight, bool& quit, std::vector<std::string>& windowModes, std::vector<glm::ivec2>& resolutions, SDL_Window* window) {
	if (mMainMenuActive && mUIScenes.mNextTabLoaded) {
		switch (mUIScenes.mActiveTab) {
		case MenuTabs::MAIN:

			if (mUIScenes.mButtonMap["PLAY"].GetPressState()) {
				mUIScenes.mNextTabLoaded = false;
				mUIScenes.mNextTab = MenuTabs::LEVELS;
				mUIScenes.mMenuMoveDirection = MenuMoveDirection::LEFT;
				mUIScenes.mActiveTab = MenuTabs::LEVELS;
			}

			if (mUIScenes.mButtonMap["QUIT"].GetPressState()) {
				quit = true;
			}

			if (mUIScenes.mButtonMap["SETTINGS"].GetPressState()) {
				mUIScenes.mNextTabLoaded = false;
				mUIScenes.mNextTab = MenuTabs::SETTINGS;
				mUIScenes.mMenuMoveDirection = MenuMoveDirection::RIGHT;
				mUIScenes.mActiveTab = MenuTabs::SETTINGS;
			}
			break;
		case MenuTabs::SETTINGS:
			if (mUIScenes.mButtonMap["BACK"].GetPressState() || ((inputManager->mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && inputManager->mLastkey_Relevant)) {
				mUIScenes.mNextTabLoaded = false;
				mUIScenes.mNextTab = MenuTabs::MAIN;	
				mUIScenes.mMenuMoveDirection = MenuMoveDirection::LEFT;
				mUIScenes.mActiveTab = MenuTabs::MAIN;

				JsonManager::saveSettings(*settings);
			}

			if (mUIScenes.mButtonMap["WINDOW_MODE"].GetPressState()) {
				if (settings->currentWindowModeIndex + 1 <= windowModes.size() - 1) {
					settings->currentWindowModeIndex += 1;
				}
				else {
					settings->currentWindowModeIndex = 0;
				}
				if (windowModes[settings->currentWindowModeIndex] == "windowed") {
					SDL_SetWindowSize(window, resolutions[settings->currentResolutionIndex].x, resolutions[settings->currentResolutionIndex].y);
					windowWidth = resolutions[settings->currentResolutionIndex].x;
					windowHeight = resolutions[settings->currentResolutionIndex].y;

					backgroundRenderer->ResizeFramebuffer(resolutions[settings->currentResolutionIndex]);

					settings->viewportOffset.x = 0;
					settings->viewportOffset.y = 0;

					glViewport(0, 0, windowWidth, windowHeight);
					SDL_SetWindowFullscreen(window, false);
				}
				else if (windowModes[settings->currentWindowModeIndex] == "fullscreen") {
					SDL_SetWindowFullscreen(window, true);

					int Width;
					int Height;

					SDL_GetWindowSize(window, &Width, &Height);

					glm::ivec2 resTemp = glm::ivec2(Width, Height);

					int i = 0;
					for (; resolutions[i].y != settings->screenSize.y; i++) {

					}

					backgroundRenderer->ResizeFramebuffer(resTemp);

					SDL_SetWindowSize(window, resTemp.x, resTemp.y);
					windowWidth = resTemp.x;
					windowHeight = resTemp.y;

					settings->viewportOffset.x = (settings->screenSize.x - windowWidth) / 2;
					settings->viewportOffset.y = (settings->screenSize.y - windowHeight) / 2;

					glViewport(settings->viewportOffset.x, settings->viewportOffset.y, windowWidth, windowHeight);

				}
			}

			if (windowModes[settings->currentWindowModeIndex] != "fullscreen") {
				if (mUIScenes.mButtonMap["RESOLUTION_DOWN"].GetPressState()) {
					if (settings->currentResolutionIndex - 1 >= 0) {
						settings->currentResolutionIndex -= 1;

						backgroundRenderer->ResizeFramebuffer(resolutions[settings->currentResolutionIndex]);

						SDL_SetWindowSize(window, resolutions[settings->currentResolutionIndex].x, resolutions[settings->currentResolutionIndex].y);
						windowWidth = resolutions[settings->currentResolutionIndex].x;
						windowHeight = resolutions[settings->currentResolutionIndex].y;

						glViewport(0, 0, windowWidth, windowHeight);
						std::cout << glm::to_string(resolutions[settings->currentResolutionIndex]) << std::endl;
					}
				}

				if (mUIScenes.mButtonMap["RESOLUTION_UP"].GetPressState()) {


					if (settings->currentResolutionIndex + 1 <= resolutions.size() - 1 && resolutions[settings->currentResolutionIndex + 1].y <= settings->screenSize.y) {
						settings->currentResolutionIndex += 1;

						backgroundRenderer->ResizeFramebuffer(resolutions[settings->currentResolutionIndex]);

						SDL_SetWindowSize(window, resolutions[settings->currentResolutionIndex].x, resolutions[settings->currentResolutionIndex].y);
						windowWidth = resolutions[settings->currentResolutionIndex].x;
						windowHeight = resolutions[settings->currentResolutionIndex].y;
						glViewport(0, 0, windowWidth, windowHeight);
						std::cout << glm::to_string(resolutions[settings->currentResolutionIndex]) << std::endl;
					}
				}
			}
			if (mUIScenes.mButtonMap["AUTO_RETRY"].GetPressState()) {
				if (settings->autoRestart) {
					settings->autoRestart = false;
				}
				else if (!settings->autoRestart) {
					settings->autoRestart = true;
				}
			}
			if (mUIScenes.mButtonMap["SFX_VOLUME_DOWN"].GetPressState()) {
				if (settings->SFXVolume > 0) {
					settings->SFXVolume -= 1;
				}
				audioHandler->SetGlobalSFXVolume(settings->SFXVolume);
			}
			if (mUIScenes.mButtonMap["SFX_VOLUME_UP"].GetPressState()) {
				if (settings->SFXVolume < 10) {
					settings->SFXVolume += 1;
				}
				audioHandler->SetGlobalSFXVolume(settings->SFXVolume);
			}
			if (mUIScenes.mButtonMap["MUSIC_VOLUME_DOWN"].GetPressState()) {
				if (settings->MusicVolume > 0) {
					settings->MusicVolume -= 1;
				}
				audioHandler->SetGlobalMusicVolume(settings->MusicVolume);
			}
			if (mUIScenes.mButtonMap["MUSIC_VOLUME_UP"].GetPressState()) {
				if (settings->MusicVolume < 10) {
					settings->MusicVolume += 1;
				}
				audioHandler->SetGlobalMusicVolume(settings->MusicVolume);
			}
			if (mUIScenes.mButtonMap["CONTROLS"].GetPressState()) {
				mUIScenes.mNextTabLoaded = false;
				mUIScenes.mNextTab = MenuTabs::CONTROLS;
				mUIScenes.mMenuMoveDirection = MenuMoveDirection::RIGHT;
				mUIScenes.mActiveTab = MenuTabs::CONTROLS;
			}
			break;
		case MenuTabs::LEVELS:
			if (mUIScenes.mButtonMap["BACK_LEVEL_TAB"].GetPressState() || ((inputManager->mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && inputManager->mLastkey_Relevant)) {
				mUIScenes.mNextTabLoaded = false;
				mUIScenes.mNextTab = MenuTabs::MAIN;
				mUIScenes.mMenuMoveDirection = MenuMoveDirection::RIGHT;
				mUIScenes.mActiveTab = MenuTabs::MAIN;
			}
			if (mUIScenes.mButtonMap["LEVEL_1"].GetPressState()) {
				LoadLevel(Levels::LEVEL_1, tilesetOffset, audioHandler->IntroMusic);
			}
			if (settings->debugMode) {
				if (mUIScenes.mButtonMap["TEST_LEVEL"].GetPressState()) {
					LoadLevel(Levels::TEST_LEVEL, tilesetOffset, audioHandler->IntroMusic);
				}
				if (mUIScenes.mButtonMap["TEST_LEVEL_OLD"].GetPressState()) {
					LoadLevel(Levels::TEST_LEVEL_OLD, tilesetOffset, audioHandler->IntroMusic);
				}
			}

			break;
		case MenuTabs::CONTROLS:
			if (mUIScenes.mButtonMap["BACK_CONTROLS_TAB"].GetPressState() || ((inputManager->mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && inputManager->mLastkey_Relevant)) {
				mUIScenes.mNextTabLoaded = false;
				mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
				mUIScenes.mNextTab = MenuTabs::SETTINGS;
				mUIScenes.mMenuMoveDirection = MenuMoveDirection::LEFT;
				mUIScenes.mActiveTab = MenuTabs::SETTINGS;
				JsonManager::saveControls(inputManager->mControls);
			}
			if (mUIScenes.mButtonMap["LEFT_BIND"].GetPressState()) {
				mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = true;
				mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
			}
			if (mUIScenes.mButtonMap["RIGHT_BIND"].GetPressState()) {
				mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = true;
				mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
			}
			if (mUIScenes.mButtonMap["JUMP_BIND"].GetPressState()) {
				mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = true;
				mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
			}
			if (mUIScenes.mButtonMap["DUCK_BIND"].GetPressState()) {
				mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = true;
			}
			break;
		}
		if ((inputManager->mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && inputManager->mLastkey_Relevant) {
			Mix_PlayChannel(18, audioHandler->EscapeClick, 0);
		}
	}

	if (!mUIScenes.mNextTabLoaded) {
		switch (mUIScenes.mMenuMoveDirection) {
		case MenuMoveDirection::LEFT:
			mUIScenes.TranslateMenuSmooth(glm::vec2(1920.0f, 0.0f), deltaTime, 3000.0f);
			break;
		case MenuMoveDirection::RIGHT:
			mUIScenes.TranslateMenuSmooth(glm::vec2(-1920.0f, 0.0f), deltaTime, 3000.0f);
			break;
		}
	}

	if (mMainMenuActive) {
		switch (mUIScenes.mActiveTab) {
		case MenuTabs::MAIN:
			break;
		case MenuTabs::SETTINGS:
			mUIScenes.mDynamicTextMap["tRESOLUTION"].TextField = std::format("{}x{}", windowWidth, windowHeight);

			mUIScenes.mDynamicTextMap["tWINDOW_MODE"].TextField = windowModes[settings->currentWindowModeIndex];

			mUIScenes.mDynamicTextMap["tMUSIC_VOLUME"].TextField = std::to_string(settings->MusicVolume);

			mUIScenes.mDynamicTextMap["tSFX_VOLUME"].TextField = std::to_string(settings->SFXVolume);

			if (settings->autoRestart) {
				mUIScenes.mDynamicTextMap["tAUTO_RETRY"].TextField = "On";
			}
			else if (!settings->autoRestart) {
				mUIScenes.mDynamicTextMap["tAUTO_RETRY"].TextField = "Off";
			}
			break;
		case MenuTabs::LEVELS:
			if (saveData->Level_1_best_levelTime_minutes == 0 && saveData->Level_1_best_levelTime_seconds == 0 && saveData->Level_1_best_levelTime_centiseconds == 0) {
				mUIScenes.mDynamicTextMap["tLEVEL_1"].TextField = "Best Time: Not played";
			}
			else {
				mUIScenes.mDynamicTextMap["tLEVEL_1"].TextField = "Best Time: " + std::format("{}:{:02}:{:02}", saveData->Level_1_best_levelTime_minutes, saveData->Level_1_best_levelTime_seconds, saveData->Level_1_best_levelTime_centiseconds);
			}
			break;
		case MenuTabs::CONTROLS:
			if (mUIScenes.mButtonMap["LEFT_BIND"].mInteracting) {
				if (inputManager->mLastkey_Relevant) {
					inputManager->mControls.LEFT_KEY_BIND = inputManager->mLastKey_Scancode;
					mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				}
				mUIScenes.mDynamicTextMap["tLEFT_BIND"].TextField = "_";
				mUIScenes.mDynamicTextMap["tLEFT_BIND"].Possition.y = 950.0f;
			}
			else {
				mUIScenes.mDynamicTextMap["tLEFT_BIND"].TextField = SDL_GetKeyName(SDL_GetKeyFromScancode(inputManager->mControls.LEFT_KEY_BIND, SDL_KMOD_NONE, false));
				mUIScenes.mDynamicTextMap["tLEFT_BIND"].Possition.y = 960.0f;
			}
			if (mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting) {
				if (inputManager->mLastkey_Relevant) {
					inputManager->mControls.RIGHT_KEY_BIND = inputManager->mLastKey_Scancode;
					mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				}
				mUIScenes.mDynamicTextMap["tRIGHT_BIND"].TextField = "_";
				mUIScenes.mDynamicTextMap["tRIGHT_BIND"].Possition.y = 850.0f;
			}
			else {
				mUIScenes.mDynamicTextMap["tRIGHT_BIND"].TextField = SDL_GetKeyName(SDL_GetKeyFromScancode(inputManager->mControls.RIGHT_KEY_BIND, SDL_KMOD_NONE, false));
				mUIScenes.mDynamicTextMap["tRIGHT_BIND"].Possition.y = 860.0f;
			}
			if (mUIScenes.mButtonMap["JUMP_BIND"].mInteracting) {
				if (inputManager->mLastkey_Relevant) {
					inputManager->mControls.JUMP_KEY_BIND = inputManager->mLastKey_Scancode;
					mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				}
				mUIScenes.mDynamicTextMap["tJUMP_BIND"].TextField = "_";
				mUIScenes.mDynamicTextMap["tJUMP_BIND"].Possition.y = 750.0f;
			}
			else {
				mUIScenes.mDynamicTextMap["tJUMP_BIND"].TextField = SDL_GetKeyName(SDL_GetKeyFromScancode(inputManager->mControls.JUMP_KEY_BIND, SDL_KMOD_NONE, false));
				mUIScenes.mDynamicTextMap["tJUMP_BIND"].Possition.y = 760.0f;
			}
			if (mUIScenes.mButtonMap["DUCK_BIND"].mInteracting) {
				if (inputManager->mLastkey_Relevant) {
					inputManager->mControls.DUCK_KEY_BIND = inputManager->mLastKey_Scancode;
					mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
				}
				mUIScenes.mDynamicTextMap["tDUCK_BIND"].TextField = "_";
				mUIScenes.mDynamicTextMap["tDUCK_BIND"].Possition.y = 650.0f;
			}
			else {
				mUIScenes.mDynamicTextMap["tDUCK_BIND"].TextField = SDL_GetKeyName(SDL_GetKeyFromScancode(inputManager->mControls.DUCK_KEY_BIND, SDL_KMOD_NONE, false));
				mUIScenes.mDynamicTextMap["tDUCK_BIND"].Possition.y = 660.0f;
			}
			break;
		}
	}
}

void SceneManager::UpdateUIInGame(bool& restart, bool& restartMode, const bool actorDead, const bool actorEscaped, bool& pause, const bool gameStarted, const glm::mat4& projectionMatrix, const glm::mat4& UImodelMatrix, float deltaTime, DeathCause actorDeathCause, ShaderProgram* textShader, GLuint pipelineID, int fps, GLuint arrayTexture) {

	if (mLevelActive) {
		if ((inputManager->mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && inputManager->mLastkey_Relevant) {
			if (pause) {
				pause = false;
				pauseTime += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pauseTimer).count();
				Mix_ResumeMusic();
				Mix_Resume(0);
				Mix_Resume(1);
				Mix_Resume(2);
				Mix_Resume(3);
				Mix_Resume(4);
				Mix_Resume(5);
				Mix_Resume(6);
				Mix_Resume(7);
				Mix_Resume(8);
				Mix_Resume(9);
				Mix_Resume(10);
				Mix_Resume(11);
				Mix_Resume(12);
				Mix_Resume(13);
				Mix_Resume(14);

				Mix_Resume(17);
				Mix_Resume(18);
				Mix_Resume(19);
			}
			else if (!pause) {
				pauseTimer = std::chrono::high_resolution_clock::now();
				pause = true;
			}
		}
	}

	if (actorDead || actorEscaped) {
		mUIScenes.mTitleScreenActive = true;
	}

	if (mLevelActive == true && mUIScenes.mTitleScreenActive == true) {

		batchRenderer->BeginBatch(projectionMatrix, &UImodelMatrix);
		batchRenderer->DrawInBatch(glm::vec2(0.0f, 0.0f), glm::vec2(1920.0f, 1080.0f), glm::vec4(0.0f, 0.0f, 0.0f, titleScreenAlpha));
		batchRenderer->EndBatch();
		batchRenderer->Flush();

		if (titleScreenAlphaTimer > titleScreenAlphaTime && titleScreenAlpha < 0.5f) {
			titleScreenAlpha += 0.008f;
			titleScreenAlphaTimer = 0.0f;
		}
		if (titleScreenAlpha >= 0.5f && !titleScreenDarkened) {
			titleScreenAlpha = 0.5f;
			titleScreenDarkened = true;
		}
		titleScreenAlphaTimer += deltaTime;

		if (titleScreenMusicVolumeTimer > titleScreenMusicVolumeTime && titleScreenMusicVolume > 0) {
			titleScreenMusicVolume -= 1;
			titleScreenMusicVolumeTimer = 0.0f;
		}
		if (titleScreenMusicVolume < 0) {
			titleScreenMusicVolume = 0;
		}
		titleScreenMusicVolumeTimer += deltaTime;

		if (titleScreenDarkened) {
			if (settings->autoRestart && !actorEscaped) {
				restart = true;
				restartMode = false;
			}

			for (const auto& pair : mUIScenes.mInGameTitleButtonMap) {
				mUIScenes.mInGameTitleButtonMap[pair.first].Render(batchRenderer, projectionMatrix, UImodelMatrix, arrayTexture);
			}

			if (!actorEscaped) {

				if (deathMessageOneShot) {
					deathMessageText;
					deathMessageColor = glm::vec4(1.0f);

					switch (actorDeathCause) {
					case DeathCause::LAVA:
						deathMessageText = "Tried to swim in lava";
						deathMessageColor = glm::vec4(1.0f, 0.42578125f, 0.12109375f, 1.0f);
						break;
					case DeathCause::FELL_DOWN:
						deathMessageText = "Fell out of the world";
						deathMessageColor = glm::vec4(0.0f, 0.7265625f, 1.0f, 1.0f);
						break;
					case DeathCause::BLACK_HOLE:
						deathMessageText = "Consumed by the void";
						deathMessageColor = glm::vec4(0.0f, 0.7265625f, 1.0f, 1.0f);
						break;
					default:
						deathMessageText = "Unknown death cause";
						break;
					}

					deathMessageOneShot = false;
				}

				mUIScenes.mInGameTitleDynamicTextMap["tTITLE"].TextField = "Game Over";
				mUIScenes.mInGameTitleDynamicTextMap["tTITLE"].Color = glm::vec3(1.0f, 0.0f, 0.14f);

				mUIScenes.mInGameTitleDynamicTextMap["tDEATH_CAUSE"].TextField = deathMessageText;
				mUIScenes.mInGameTitleDynamicTextMap["tDEATH_CAUSE"].Color = deathMessageColor;

				mUIScenes.mInGameTitleDynamicTextMap["tTIME"].TextField = "";

				mUIScenes.mInGameTitleDynamicTextMap["tBEST_TIME"].TextField = "";

				mUIScenes.mInGameTitleDynamicTextMap["tNEW_BEST_TIME"].TextField = "";

				mUIScenes.mInGameTitleDynamicTextMap["tRETRY"].TextField = "Retry";
			}
			else {
				if (((levelTime_minutes * 6000 + levelTime_seconds * 100 + levelTime_centiseconds) < (saveData->Level_1_best_levelTime_minutes * 6000 + saveData->Level_1_best_levelTime_seconds * 100 + saveData->Level_1_best_levelTime_centiseconds)) || (saveData->Level_1_best_levelTime_minutes == 0 && saveData->Level_1_best_levelTime_seconds == 0 && saveData->Level_1_best_levelTime_centiseconds == 0)) {
					saveData->Level_1_best_levelTime_minutes = levelTime_minutes;
					saveData->Level_1_best_levelTime_seconds = levelTime_seconds;
					saveData->Level_1_best_levelTime_centiseconds = levelTime_centiseconds;
					JsonManager::saveSaveData(*saveData);
					mNewBestTimeMessageOneShot = true;
				}

				mUIScenes.mInGameTitleDynamicTextMap["tTITLE"].TextField = "Escaped";
				mUIScenes.mInGameTitleDynamicTextMap["tTITLE"].Color = glm::vec3(0.8125f, 0.0f, 0.875f);

				mUIScenes.mInGameTitleDynamicTextMap["tDEATH_CAUSE"].TextField = "";

				mUIScenes.mInGameTitleDynamicTextMap["tTIME"].TextField = "Time: " + std::format("{}:{:02}:{:02}", levelTime_minutes, levelTime_seconds, levelTime_centiseconds);

				mUIScenes.mInGameTitleDynamicTextMap["tBEST_TIME"].TextField = "Best Time: " + std::format("{}:{:02}:{:02}", saveData->Level_1_best_levelTime_minutes, saveData->Level_1_best_levelTime_seconds, saveData->Level_1_best_levelTime_centiseconds);

				if (mNewBestTimeMessageOneShot) {
					mUIScenes.mInGameTitleDynamicTextMap["tNEW_BEST_TIME"].TextField = "New Best Time!";
				}
				else {
					mUIScenes.mInGameTitleDynamicTextMap["tNEW_BEST_TIME"].TextField = "";
				}

				mUIScenes.mInGameTitleDynamicTextMap["tRETRY"].TextField = "Restart";


			}
			if (mUIScenes.mInGameTitleButtonMap["RETRY"].GetPressState()) {
				restart = true;
				restartMode = false;
			}
			if (mUIScenes.mInGameTitleButtonMap["AUTO_RETRY"].GetPressState()) {
				if (settings->autoRestart) {
					settings->autoRestart = false;
				}
				else if (!settings->autoRestart) {
					settings->autoRestart = true;
				}
				JsonManager::saveSettings(*settings);
			}
			if (mUIScenes.mInGameTitleButtonMap["BACK_TO_MENU"].GetPressState()) {
				restart = true;
				restartMode = true;
			}

			if (settings->autoRestart) {
				mUIScenes.mInGameTitleDynamicTextMap["tAUTO_RETRY"].TextField = "On";
			}
			else if (!settings->autoRestart) {
				mUIScenes.mInGameTitleDynamicTextMap["tAUTO_RETRY"].TextField = "Off";
			}

			for (int i = 0; i < mUIScenes.mInGameTitleTextToRender.size(); i++) {
				textRenderer->RenderText(textShader, pipelineID, mUIScenes.mInGameTitleTextToRender[i].TextField, mUIScenes.mInGameTitleTextToRender[i].Possition.x, mUIScenes.mInGameTitleTextToRender[i].Possition.y, mUIScenes.mInGameTitleTextToRender[i].Scale, mUIScenes.mInGameTitleTextToRender[i].Color, projectionMatrix, *mUIScenes.mInGameTitleTextToRender[i].ModelMatrix, mUIScenes.mInGameTitleTextToRender[i].CenteredX, mUIScenes.mInGameTitleTextToRender[i].CenteredY, mUIScenes.mInGameTitleTextToRender[i].RightSided);
			}

			for (auto& pair : mUIScenes.mInGameTitleDynamicTextMap) {
				textRenderer->RenderText(textShader, pipelineID, mUIScenes.mInGameTitleDynamicTextMap[pair.first].TextField, mUIScenes.mInGameTitleDynamicTextMap[pair.first].Possition.x, mUIScenes.mInGameTitleDynamicTextMap[pair.first].Possition.y, mUIScenes.mInGameTitleDynamicTextMap[pair.first].Scale, mUIScenes.mInGameTitleDynamicTextMap[pair.first].Color, projectionMatrix, *mUIScenes.mInGameTitleDynamicTextMap[pair.first].ModelMatrix, mUIScenes.mInGameTitleDynamicTextMap[pair.first].CenteredX, mUIScenes.mInGameTitleDynamicTextMap[pair.first].CenteredY, mUIScenes.mInGameTitleDynamicTextMap[pair.first].RightSided);
			}
		}
	}
	else if (mLevelActive && pause) {
		Mix_PauseMusic();
		Mix_Pause(0);
		Mix_Pause(1);
		Mix_Pause(2);
		Mix_Pause(3);
		Mix_Pause(4);
		Mix_Pause(5);
		Mix_Pause(6);
		Mix_Pause(7);
		Mix_Pause(8);
		Mix_Pause(9);
		Mix_Pause(10);
		Mix_Pause(11);
		Mix_Pause(12);
		Mix_Pause(13);
		Mix_Pause(14);

		Mix_Pause(17);
		Mix_Pause(18);
		Mix_Pause(19);

		batchRenderer->BeginBatch(projectionMatrix, &UImodelMatrix);
		batchRenderer->DrawInBatch(glm::vec2(0.0f, 0.0f), glm::vec2(1920.0f, 1080.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
		batchRenderer->EndBatch();
		batchRenderer->Flush();

		for (const auto& pair : mUIScenes.mInGamePauseButtonMap) {
			mUIScenes.mInGamePauseButtonMap[pair.first].Render(batchRenderer, projectionMatrix, UImodelMatrix, arrayTexture);
		}
		if (mUIScenes.mInGamePauseButtonMap["CONTINUE"].GetPressState()) {
			pause = false;
			pauseTime += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pauseTimer).count();
			Mix_ResumeMusic();
			Mix_Resume(0);
			Mix_Resume(1);
			Mix_Resume(2);
			Mix_Resume(3);
			Mix_Resume(4);
			Mix_Resume(5);
			Mix_Resume(6);
			Mix_Resume(7);
			Mix_Resume(8);
			Mix_Resume(9);
			Mix_Resume(10);
			Mix_Resume(11);
			Mix_Resume(12);
			Mix_Resume(13);
			Mix_Resume(14);

			Mix_Resume(17);
			Mix_Resume(18);
			Mix_Resume(19);
		}
		if (mUIScenes.mInGamePauseButtonMap["AUTO_RETRY"].GetPressState()) {
			if (settings->autoRestart) {
				settings->autoRestart = false;
			}
			else if (!settings->autoRestart) {
				settings->autoRestart = true;
			}
			JsonManager::saveSettings(*settings);
		}
		if (mUIScenes.mInGamePauseButtonMap["RESTART"].GetPressState()) {
			pause = false;
			restart = true;
			restartMode = false;
		}
		if (mUIScenes.mInGamePauseButtonMap["BACK_TO_MENU"].GetPressState()) {
			pause = false;
			restart = true;
			restartMode = true;
		}
		if (settings->autoRestart) {
			mUIScenes.mInGamePauseDynamicTextMap["tAUTO_RETRY"].TextField = "On";
		}
		else if (!settings->autoRestart) {
			mUIScenes.mInGamePauseDynamicTextMap["tAUTO_RETRY"].TextField = "Off";
		}

		for (int i = 0; i < mUIScenes.mInGamePauseTextToRender.size(); i++) {
			textRenderer->RenderText(textShader, pipelineID, mUIScenes.mInGamePauseTextToRender[i].TextField, mUIScenes.mInGamePauseTextToRender[i].Possition.x, mUIScenes.mInGamePauseTextToRender[i].Possition.y, mUIScenes.mInGamePauseTextToRender[i].Scale, mUIScenes.mInGamePauseTextToRender[i].Color, projectionMatrix, *mUIScenes.mInGamePauseTextToRender[i].ModelMatrix, mUIScenes.mInGamePauseTextToRender[i].CenteredX, mUIScenes.mInGamePauseTextToRender[i].CenteredY, mUIScenes.mInGamePauseTextToRender[i].RightSided);
		}
		for (auto& pair : mUIScenes.mInGamePauseDynamicTextMap) {
			textRenderer->RenderText(textShader, pipelineID, mUIScenes.mInGamePauseDynamicTextMap[pair.first].TextField, mUIScenes.mInGamePauseDynamicTextMap[pair.first].Possition.x, mUIScenes.mInGamePauseDynamicTextMap[pair.first].Possition.y, mUIScenes.mInGamePauseDynamicTextMap[pair.first].Scale, mUIScenes.mInGamePauseDynamicTextMap[pair.first].Color, projectionMatrix, *mUIScenes.mInGamePauseDynamicTextMap[pair.first].ModelMatrix, mUIScenes.mInGamePauseDynamicTextMap[pair.first].CenteredX, mUIScenes.mInGamePauseDynamicTextMap[pair.first].CenteredY, mUIScenes.mInGamePauseDynamicTextMap[pair.first].RightSided);
		}
	}

	levelTimeTotal = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - levelTimerTotal).count();
	if (gameStarted && !actorEscaped && !mUIScenes.mTitleScreenActive) {
		if (pause) {
			levelTime = levelTimeTotal - (pauseTime + std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pauseTimer).count());
		}
		else {
			levelTime = levelTimeTotal - pauseTime;
		}
		if (!pause) {
			levelTime_seconds = (levelTime / 1000) % 60;

			levelTime_minutes = (levelTime / 1000) / 60;

			levelTime_centiseconds = (levelTime % 1000) / 10;
		}
		if (settings->showFpsInGame) {
			textRenderer->RenderText(textShader, pipelineID, "FPS: " + std::to_string(fps), 80.0f, 1000.0f, 0.20f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), projectionMatrix, UImodelMatrix, false, false, false);
		}

		textRenderer->RenderText(textShader, pipelineID, std::format("{}:{:02}:{:02}", levelTime_minutes, levelTime_seconds, levelTime_centiseconds), 1840.0f, 1000.0f, 0.35f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), projectionMatrix, UImodelMatrix, false, false, true);
	}
}

void SceneManager::ResetTimer() {
	pauseTime = 0;
	levelTime = 0;
	levelTimeTotal = 0;

	titleScreenDarkened = false;

	titleScreenAlpha = 0.0f;
	titleScreenMusicVolume = 128;
	titleScreenAlphaTimer = 0.0f;

	levelTime_seconds = 0;
	levelTime_minutes = 0;
	levelTime_centiseconds = 0;
}

void SceneManager::StartLevelTimer() {
	levelTimerTotal = std::chrono::high_resolution_clock::now();
}

void SceneManager::Init(InputManager* INinputManager, BackgroundRenderer* INbackgroundRenderer, AudioHandler* INaudioHandler, Settings* INsettings, SaveData* INsaveData, BatchRenderer* INbatchRenderer, TextRenderer* INtextRenderer) {
	saveData = INsaveData;
	settings = INsettings;
	inputManager = INinputManager;
	audioHandler = INaudioHandler;
	textRenderer = INtextRenderer;
	batchRenderer = INbatchRenderer;
	backgroundRenderer = INbackgroundRenderer;
}

void SceneManager::LoadMainMenu(const int& tilesetOffset) {

	mUIScenes.LoadMainMenuTab(tilesetOffset);
	mUIScenes.FillBlockVector();

	mCurrentBlocks = &mUIScenes.mMainMenuBlocks;
	mLevelActive = false;
	mMainMenuActive = true;
}


void SceneManager::ReloadCurrentLevel(const int tilesetOffset, Mix_Music* introMusic) {
	switch (mCurrentLevel) {
	case Levels::LEVEL_1:
		mLevelScene.mLevelBlocks.clear();
		mLevelScene.LoadLevel("levels/GameLevels/32p/Level_1.json", tilesetOffset);
		mCurrentBlocks = &mLevelScene.mLevelBlocks;
		mLevelActive = true;
		mMainMenuActive = false;
		break;
	case Levels::TEST_LEVEL:
		mLevelScene.mLevelBlocks.clear();
		mLevelScene.LoadLevel("levels/GameLevels/32p/Test_Level_V2.json", tilesetOffset);
		mCurrentBlocks = &mLevelScene.mLevelBlocks;
		mLevelActive = true;
		mMainMenuActive = false;
		break;
	case Levels::TEST_LEVEL_OLD:
		mLevelScene.mLevelBlocks.clear();
		mLevelScene.LoadLevel("levels/GameLevels/32p/Test_Level.json", tilesetOffset);
		mCurrentBlocks = &mLevelScene.mLevelBlocks;
		mLevelActive = true;
		mMainMenuActive = false;
		break;
	default:
		std::cerr << "Error: ReloadCurrentLevel: Invalid current level" << std::endl;
		break;
	}
	deathMessageOneShot = true;
	Mix_HaltMusic();
	Mix_PlayMusic(introMusic, 0);
}

void SceneManager::LoadLevel(Levels level, const int tilesetOffset, Mix_Music* introMusic) {
	mCurrentLevel = level;
	ReloadCurrentLevel(tilesetOffset, introMusic);
}
