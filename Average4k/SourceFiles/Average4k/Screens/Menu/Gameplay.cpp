/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "Gameplay.h"
#include "MainMenu.h"
#include "../../Api/Functions/FGameplay.h"
#include <AvgEngine/Utils/Paths.h>

void Average4k::Screens::Menu::Gameplay::loadChart()
{
	if (_path.ends_with(L".sm") || _path.ends_with(L".ssc"))
		chart = Average4k::Data::Chart::Providers::StepFile(_path);
}

void Average4k::Screens::Menu::Gameplay::loadBackground()
{
	std::wstring folder = chart.path.substr(0, chart.path.find_last_of(L"/\\"));
	std::wstring bgPath = folder + L"/" + chart.metadata.background;

	std::string strPath = std::string(bgPath.begin(), bgPath.end());

	if (!AvgEngine::Utils::Paths::pathExists(strPath))
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Warning] Background not found.");
		return;
	}

	background = new AvgEngine::Base::Sprite(0,0, strPath);

	background->tag = "background";

	// set width and height

	background->transform.w = AvgEngine::Render::Display::width;
	background->transform.h = AvgEngine::Render::Display::height;

	background->transform.a = save->gameplayData.backgroundDim;

	// add
	addObject(background);
}

void Average4k::Screens::Menu::Gameplay::loadPlayfield()
{
	std::string p = "Assets/Noteskin/" + save->gameplayData.noteskin + "/skin.lua";
	std::string png = "Assets/Noteskin/" + save->gameplayData.noteskin + "/noteskin.png";

	if (!AvgEngine::Utils::Paths::pathExists(p))
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] Noteskin lua file (" + p + ") not found, returning to main menu.");
		AvgEngine::Game::Instance->SwitchMenu(new MainMenu("Scripts/MainMenu.lua"));
		return;
	}

	if (!AvgEngine::Utils::Paths::pathExists(png))
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] Noteskin texture (" + png + ") not found, returning to main menu.");
		AvgEngine::Game::Instance->SwitchMenu(new MainMenu("Scripts/MainMenu.lua"));
		return;
	}

	_noteskinSheet = AvgEngine::OpenGL::Texture::createWithImage(png);

	_noteskinSheet->dontDelete = true;

	lua = new Average4k::Api::AvgLuaFile(p);
	lua->path = p;

	Average4k::Api::Stubs::LuaMenu::Register(lua->getState());

	Average4k::Api::Stubs::LuaMenu cm = Average4k::Api::Stubs::LuaMenu(this);

	lua->getState().set("currentMenu", cm);

	auto setup = lua->getState().get<sol::optional<sol::protected_function>>("setup");

	lua->getState().set_function("setComboTag", Average4k::Api::Functions::FGameplay::SetComboTag);
	lua->getState().set_function("setJudgementTag", Average4k::Api::Functions::FGameplay::SetJudgementTag);
	lua->getState().set_function("setJudgementTextTag", Average4k::Api::Functions::FGameplay::SetJudgementTextTag);
	lua->getState().set_function("setNoteSize", Average4k::Api::Functions::FGameplay::SetNoteSize);
	lua->getState().set_function("setReceptorLocation", Average4k::Api::Functions::FGameplay::SetReceptorLocation);

	lua->getState().set_function("getWidthScale", Average4k::Api::Functions::FGame::GetWidthScale);
	lua->getState().set_function("getHeightScale", Average4k::Api::Functions::FGame::GetHeightScale);

	if (setup.has_value())
	{
		sol::protected_function_result result = setup.value()();

		if (!result.valid())
		{
			sol::error err = result;
			AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + err.what());
		}
	}
	else
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No setup function found.");
		AvgEngine::Game::Instance->SwitchMenu(new MainMenu("Scripts/MainMenu.lua"));
		return;
	}

	// load receptors

	noteWidth = noteWidth / _noteskinSheet->width; // opengl normalizes the texture coordinates
	noteHeight = noteHeight / _noteskinSheet->height;

	for (int i = 0; i < 4; i++)
	{
		AvgEngine::Base::Sprite* spr = new AvgEngine::Base::Sprite(0, 0, _noteskinSheet);
		spr->tag = "receptor_" + std::to_string(i);
		spr->transform.x = (i * (AvgEngine::Render::Display::width / 4)) + (AvgEngine::Render::Display::width / 8);
		spr->transform.y = AvgEngine::Render::Display::height - 100;
		spr->transform.w = 256;
		spr->transform.h = 256;

		spr->src.x = noteWidth * receptorColumn;
		spr->src.w = noteWidth;
		spr->src.y = noteHeight * receptorRow;
		spr->src.h = noteHeight;

		receptors.push_back(spr);
	}
}

void Average4k::Screens::Menu::Gameplay::start()
{
}

void Average4k::Screens::Menu::Gameplay::load()
{
	hud = new Average4k::Objects::RenderTexture(&camera, AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);

	hud_spr = new AvgEngine::Base::Sprite(0, 0, hud->texture);

	// Stop all audio
	for (AvgEngine::Audio::Channel* c : AvgEngine::External::BASS::Channels)
	{
		if (c == NULL || c->id == -1)
			continue;
		if (c->isPlaying)
			c->Stop();
		delete c;
	}

	AvgEngine::External::BASS::Channels.clear();

	save = &A4kGame::gameInstance->saveData;
	loadChart();
	if (!chart.isValid)
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] Chart is not valid, returning to main menu.");
		AvgEngine::Game::Instance->SwitchMenu(new MainMenu("Scripts/MainMenu.lua"));
		return;
	}

	AvgEngine::Logging::writeLog("[Gameplay] Playing " + std::string(chart.metadata.title.begin(), chart.metadata.title.end()) + " on " + std::string(chart.difficulties[_diff].name.begin(), chart.difficulties[_diff].name.end()));

	loadBackground();

	loadPlayfield();

	// event handlers

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
		if (e.data == save->keybindData.keyPause)
			AvgEngine::Game::Instance->SwitchMenu(new MainMenu("Scripts/MainMenu.lua"));
	});
}

void Average4k::Screens::Menu::Gameplay::draw()
{
	// draw bg
	background->draw();

	// draw playfield and hud
	hud->Bind();

	for (AvgEngine::Base::GameObject* o : GameObjects)
	{
		if (o->tag == "background")
			continue;
		o->draw();
	}

	for (AvgEngine::Base::Sprite* s : receptors)
	{
		s->draw();
	}

	hud->Unbind();

	hud_spr->draw();
}
