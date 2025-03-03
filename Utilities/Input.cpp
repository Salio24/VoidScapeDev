#include "Input.hpp"
#include "../App.hpp"

InputManager::InputManager() : app_(nullptr) {

}

InputManager::~InputManager() {

}

App& InputManager::app() {
	if (app_ == nullptr) {
		app_ = &App::getInstance();
	}
	return *app_;
}

void InputManager::Input() {
	// temp 
	static bool ones1 = true;
	static bool ones2 = true;


	SDL_Event e;
	LClick = false;
	mLastkey_Relevant = false;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_EVENT_QUIT) {
			std::cout << "Goodbye!" << std::endl;
			app().mQuit = true;
		}
		else if (e.type == SDL_EVENT_MOUSE_MOTION) {
			mousePos.x = (e.motion.x / (float)app().mWindowWidth - (float)app().mSettings.viewportOffset.x / (float)app().mWindowWidth) * 1920.0f;
			mousePos.y = (1.0f - e.motion.y / (float)app().mWindowHeight + (float)app().mSettings.viewportOffset.y / (float)app().mWindowHeight) * 1080.0f;
			//680.680 
			//1000.760

			//std::cout << "Mouse motion: " << glm::to_string(mousePos) << (float)app().viewportOffset.x / (float)app().mWindowWidth << std::endl;
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				LClick = true;
			}
		}
		if (e.type == SDL_EVENT_KEY_DOWN) {
			mLastkey_Relevant = true;
			mLastKey_Keycode = e.key.key;
			mLastKey_Scancode = e.key.scancode;

			if (e.key.scancode == SDL_SCANCODE_MINUS && ones1) {
				app().mCamera.mCameraScale -= 0.0125f;
				app().mCamera.mCameraScale = std::clamp(app().mCamera.mCameraScale, 0.6f, 2.0f);
				app().mCamera.RescaleCamera(app().mCamera.mCameraScale);
				ones1 = false;
			} else if (e.key.scancode == SDL_SCANCODE_EQUALS && ones2) {
				app().mCamera.mCameraScale += 0.0125f;
				app().mCamera.mCameraScale = std::clamp(app().mCamera.mCameraScale, 0.6f, 2.0f);
				app().mCamera.RescaleCamera(app().mCamera.mCameraScale);
				ones2 = false;
			}
		}
		if (e.type == SDL_EVENT_KEY_UP) {

			if (e.key.scancode == mControls.JUMP_KEY_BIND) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::SPACE)] = false;
				app().mMovementHandler.mSpacebarOneShot = true;
				app().mPhysicsComponent.testButton1 = false;
			} else if (e.key.scancode == mControls.DUCK_KEY_BIND) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::DUCK)] = false;
				app().mMovementHandler.mDuckOneShot = true;
				app().mPhysicsComponent.testButton2 = false;
			} else if (e.key.scancode == mControls.LEFT_KEY_BIND) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] = false;
				app().mPhysicsComponent.testButton3 = false;
			} else if (e.key.scancode == mControls.RIGHT_KEY_BIND) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] = false;
				app().mPhysicsComponent.testButton4 = false;
			} 
			else if (e.key.scancode == SDL_SCANCODE_MINUS) {
				ones1 = true;
			}
			else if (e.key.scancode == SDL_SCANCODE_EQUALS) {
				ones2 = true;
			}

		}
		const bool* state = SDL_GetKeyboardState(nullptr);
		if (state[mControls.JUMP_KEY_BIND]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::SPACE)] = true;
			app().mPhysicsComponent.testButton1 = true;
		}
		if (state[mControls.DUCK_KEY_BIND]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::DUCK)] = true;
			app().mPhysicsComponent.testButton2 = true;
		}
		if (state[mControls.LEFT_KEY_BIND]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] = true;
			app().mPhysicsComponent.testButton3 = true;
		}
		if (state[mControls.RIGHT_KEY_BIND]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] = true;
			app().mPhysicsComponent.testButton4 = true;
		}
		if (app().mSettings.debugMode) {
			if (state[SDL_SCANCODE_T]) {
				app().LoadGame(true);
			}
			if (state[SDL_SCANCODE_GRAVE]) {
				app().mQuit = true;
			}
			if (state[SDL_SCANCODE_Y]) {
				app().testReset = true;
			}
			if (state[SDL_SCANCODE_K]) {
				app().mPhysicsComponent.testV += glm::vec2(1.0f, 1.0f) * 5.0f;
				app().testSpeeed += 5.0f;
			}
			if (state[SDL_SCANCODE_I]) {
				app().mPhysicsComponent.testV += glm::vec2(1.0f, 1.0f) * -5.0f;
				app().testSpeeed -= 5.0f;
			}
			if (state[SDL_SCANCODE_L]) {
				app().mPhysicsComponent.testV = glm::vec2(0.0f, 0.0f);
				app().testSpeeed = 0.0f;

			}
			if (state[SDL_SCANCODE_H]) {
				app().ReloadConfig();
			}
		}
	}

	if (app().mSceneManager.mMainMenuActive == true) {
		for (const auto& pair : app().mSceneManager.mUIScenes.mButtonMap) {
			if (pair.second.mCreated) {
				if (PointVsRect((glm::vec2)mousePos, pair.second.mTriggerSize, pair.second.mTriggerPos)) {
					app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetHoverState(true);

					if (app().mSceneManager.mUIScenes.mButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mButtonMap[pair.first].ChangeColor(glm::vec3(1.0f));
					}


					if (LClick) {
						app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetPressState(true);
					}
					else {
						app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetPressState(false);
					}
				}
				else {
					app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetHoverState(false);
					app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetPressState(false);
					if (app().mSceneManager.mUIScenes.mButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mButtonMap[pair.first].ChangeColor(glm::vec3(0.7f));
					}
				}
			}
		}		
	}
	else if (app().mSceneManager.mUIScenes.mTitleScreenActive == true) {
		for (const auto& pair : app().mSceneManager.mUIScenes.mInGameTitleButtonMap) {
			if (pair.second.mCreated) {
				if (PointVsRect((glm::vec2)mousePos, pair.second.mTriggerSize, pair.second.mTriggerPos)) {
					app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetHoverState(true);

					if (app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].ChangeColor(glm::vec3(1.0f));
					}


					if (LClick) {
						app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetPressState(true);
					}
					else {
						app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetPressState(false);
					}
				}
				else {
					app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetHoverState(false);
					app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetPressState(false);
					if (app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].ChangeColor(glm::vec3(0.7f));
					}
				}
			}
		}
	}
	else if (app().mPause) {
		for (const auto& pair : app().mSceneManager.mUIScenes.mInGamePauseButtonMap) {
			if (pair.second.mCreated) {
				if (PointVsRect((glm::vec2)mousePos, pair.second.mTriggerSize, pair.second.mTriggerPos)) {
					app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetHoverState(true);

					if (app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].ChangeColor(glm::vec3(1.0f));
					}


					if (LClick) {
						app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetPressState(true);
					}
					else {
						app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetPressState(false);
					}
				}
				else {
					app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetHoverState(false);
					app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetPressState(false);
					if (app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].ChangeColor(glm::vec3(0.7f));
					}
				}
			}
		}
	}




}