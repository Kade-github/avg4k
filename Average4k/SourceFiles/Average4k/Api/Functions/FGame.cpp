/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FGame.h"

#include <GLFW/glfw3.h>
#include <AvgEngine/Game.h>

void Average4k::Api::Functions::FGame::Quit()
{
	glfwSetWindowShouldClose(AvgEngine::Game::Instance->Window, true);
}