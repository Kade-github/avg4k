/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FGameplay.h"
#include "../../Screens/Menu/Gameplay.h"

void Average4k::Api::Functions::FGameplay::RotateReceptors(bool rotate)
{
	Average4k::Screens::Menu::Gameplay* gameplay = (Average4k::Screens::Menu::Gameplay*)A4kGame::gameInstance->CurrentMenu;

	gameplay->rotateReceptors = rotate;
}

void Average4k::Api::Functions::FGameplay::SetNoteSize(float width, float height)
{
	Average4k::Screens::Menu::Gameplay* gameplay = (Average4k::Screens::Menu::Gameplay*)A4kGame::gameInstance->CurrentMenu;

	gameplay->noteWidth = width;
	gameplay->noteHeight = height;
}