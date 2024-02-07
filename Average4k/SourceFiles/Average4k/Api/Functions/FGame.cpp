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

float Average4k::Api::Functions::FGame::GetWidthScale()
{
	return AvgEngine::Render::Display::width / 1920.0f;
}

float Average4k::Api::Functions::FGame::GetHeightScale()
{
	return AvgEngine::Render::Display::height / 1080.0f;
}
