/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FGameplay.h"
#include "../../Screens/Menu/Gameplay.h"

void Average4k::Api::Functions::FGameplay::SetNoteSize(float width, float height)
{
	Average4k::Screens::Menu::Gameplay* gameplay = (Average4k::Screens::Menu::Gameplay*)A4kGame::gameInstance->CurrentMenu;

	gameplay->noteWidth = width;
	gameplay->noteHeight = height;
}

void Average4k::Api::Functions::FGameplay::SetAccuracyTag(std::string tag)
{
	Average4k::Screens::Menu::Gameplay* gameplay = (Average4k::Screens::Menu::Gameplay*)A4kGame::gameInstance->CurrentMenu;

	gameplay->accuracyTag = tag;
}

void Average4k::Api::Functions::FGameplay::SetComboTag(std::string tag)
{
	Average4k::Screens::Menu::Gameplay* gameplay = (Average4k::Screens::Menu::Gameplay*)A4kGame::gameInstance->CurrentMenu;

	gameplay->comboTag = tag;
}

void Average4k::Api::Functions::FGameplay::SetJudgementTag(std::string judge, std::string tag)
{
	Average4k::Screens::Menu::Gameplay* gameplay = (Average4k::Screens::Menu::Gameplay*)A4kGame::gameInstance->CurrentMenu;

	gameplay->judgementTags[judge] = tag;
}

void Average4k::Api::Functions::FGameplay::SetJudgementTextTag(std::string tag)
{
	Average4k::Screens::Menu::Gameplay* gameplay = (Average4k::Screens::Menu::Gameplay*)A4kGame::gameInstance->CurrentMenu;

	gameplay->judgementTextTag = tag;
}
