#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "HelperStructs.hpp"

class App;

class InputManager {
public:
	InputManager();

	~InputManager();

	void Input();

	glm::ivec2 mousePos;

	bool mLastkey_Relevant{ false };

	SDL_Keycode mLastKey_Keycode{ SDLK_UNKNOWN };

	SDL_Scancode mLastKey_Scancode{ SDL_SCANCODE_UNKNOWN };

	Controls mControls;

private:
	App& app();

	App* app_;

	bool LClick{ false };
};