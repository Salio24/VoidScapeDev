#include "App.hpp"
#include <SDL3/SDL_hints.h>

int main(int argc, char* argv[]) {
	App& app = App::getInstance();

	app.PostStartUp();
	app.LoadGame();
	app.MainLoop();
	return 0;
}