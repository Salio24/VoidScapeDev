// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "App.hpp"

int main(int argc, char* argv[]) {
	App& app = App::getInstance();

	app.PostStartUp();
	app.LoadGame();
	app.MainLoop();
	return 0;
}