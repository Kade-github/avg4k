/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FGameplay.h"
#include "../../Screens/Menu/Gameplay.h"

void Average4k::Api::Functions::FGameplay::RotateReceptors(bool rotate)
{
	std::shared_ptr<Average4k::Screens::Menu::Gameplay> gameplay = std::dynamic_pointer_cast<Average4k::Screens::Menu::Gameplay>(A4kGame::gameInstance->CurrentMenu);

	gameplay->rotateReceptors = rotate;
}

void Average4k::Api::Functions::FGameplay::SetNoteSize(float width, float height)
{
	std::shared_ptr< Average4k::Screens::Menu::Gameplay> gameplay = std::dynamic_pointer_cast<Average4k::Screens::Menu::Gameplay>(A4kGame::gameInstance->CurrentMenu);

	gameplay->noteWidth = width;
	gameplay->noteHeight = height;
}