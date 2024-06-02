/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "Gameplay.h"
#include "MainMenu.h"

#include "../../Api/Functions/FGameplay.h"
#include "../../Api/Stubs/LuaNote.h"
#include "../../Helpers/JudgementHelper.h"

#include <AvgEngine/Utils/Paths.h>
#include "../../Steam/UGCHandler.h"
#include "../../Helpers/SteamHelper.h"

void Average4k::Screens::Menu::Gameplay::restart()
{
	hasStarted = false;
	channel->Stop();
	for (auto&& n : notes)
	{
		removeObject(n);
		n->texture->dontDelete = true;
	}
	notes.clear();

	start();
}

void Average4k::Screens::Menu::Gameplay::leave()
{
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

	stop = true;

	notes.clear();
	receptors.clear();

	AvgEngine::Game::Instance->SwitchMenu(std::make_shared<Average4k::Screens::Menu::MainMenu>("Scripts/MainMenu.lua"));
}

void Average4k::Screens::Menu::Gameplay::noteRelease(int lane)
{
	holdingLanes[lane] = 0;

	for (auto n : notes)
	{
		if (n->data.lane != lane)
			continue;

		if (n->data.type != Data::Chart::Head)
			continue;

		std::shared_ptr<Average4k::Objects::HoldNote> hn = std::dynamic_pointer_cast<Average4k::Objects::HoldNote>(n);
		hn->holding = false;
		hn->endHold = currentBeat;
	}

	lua->getState().collect_garbage();
	sol::protected_function_result result = receptorRelease(Average4k::Api::Stubs::LuaSprite(receptors[lane]), noteWidth, noteHeight);

	if (!result.valid())
	{
		sol::error err = result;
		AvgEngine::Logging::writeLog("[Lua] [Warning] Error in function receptorRelease.\n" + std::string(err.what()));
	}
}

void Average4k::Screens::Menu::Gameplay::noteHit(int lane)
{
	lua->getState().collect_garbage();
	sol::protected_function_result result = receptorHit(Average4k::Api::Stubs::LuaSprite(receptors[lane]), noteWidth, noteHeight);


	holdingLanes[lane] = 1;

	if (!result.valid())
	{
		sol::error err = result;
		AvgEngine::Logging::writeLog("[Lua] [Warning] Error in function receptorHit.\n" + std::string(err.what()));
	}

	std::shared_ptr<Average4k::Objects::BaseNote> hit = nullptr;
	std::string j = "Miss";

	for (auto&& n : notes)
	{
		if (n->data.lane != lane || n->data.type == Data::Chart::Fake || n->data.type == Data::Chart::Mine)
			continue;

		std::string judgement = Average4k::Helpers::JudgementHelper::GetJudgement(currentTime - n->noteTime);

		if (judgement == "Miss")
			continue;

		if (hit == nullptr)
		{
			hit = n;
			j = judgement;
		}
		else
		{
			float d = std::abs(n->noteTime - currentTime);
			float d2 = std::abs(hit->noteTime - currentTime);

			if (d < d2)
			{
				hit = n;
				j = judgement;
			}
		}

	}

	if (hit == nullptr)
		return;

	bool hitBefore = hit->hit;

	if ((j == "Miss" && !hitBefore))
		return;

	std::shared_ptr<Average4k::Objects::HoldNote> hn = nullptr;
	switch (hit->data.type)
	{
	case Data::Chart::Tap:
		if (hitBefore)
			return;
		hit->hit = true;
		break;
	case Data::Chart::Head:
		hn = std::dynamic_pointer_cast<Average4k::Objects::HoldNote>(hit);
		hit->hit = true;
		hn->holding = true;
		break;
	}

	if (j == "Miss")
		return;

	if (hitBefore)
		return;

	if (botplay)
		j = "Perfect";

	if (hit->hit)
	{
		combo++;
		totalNotes++;

		if (j == "Marvelous")
			hitNotes++;
		else if (j == "Perfect")
			hitNotes += 0.925;
		else if (j == "Great")
			hitNotes += 0.7;
		else if (j == "Good")
		{
			hitNotes += 0.35;
		}
		else if (j == "Bad")
		{
			combo = 0;
			hitNotes += 0.1;
		}

		sol::protected_function_result result = hitNote(hit->data.type, hit->data.lane, j, combo, (float)hitNotes / (float)totalNotes);



		if (!result.valid())
		{
			sol::error err = result;
			AvgEngine::Logging::writeLog("[Lua] [Warning] Error in function hitNote for note type " + std::to_string(hit->data.type) + " at beat " + std::to_string(hit->data.beat) + ".\n" + std::string(err.what()));
		}
	}
}

void Average4k::Screens::Menu::Gameplay::noteMiss()
{
	combo = 0;
	totalNotes++;

	sol::protected_function_result result = missNote(combo, (float)hitNotes / (float)totalNotes);

	if (!result.valid())
	{
		sol::error err = result;
		AvgEngine::Logging::writeLog("[Lua] [Warning] Error in function missNote.\n" + std::string(err.what()));
	}
}

void Average4k::Screens::Menu::Gameplay::loadAudio()
{
	std::wstring folder = chart.path.substr(0, chart.path.find_last_of(L"/\\"));
	std::wstring audioPath = folder + L"/" + chart.metadata.file;

	std::string strPath = std::string(audioPath.begin(), audioPath.end());

	if (!AvgEngine::Utils::Paths::pathExists(strPath))
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Warning] Audio not found.");
		return;
	}

	channel = AvgEngine::External::BASS::CreateChannel("audio", strPath, false);
	channel->ConvertToFX();

	channel->length = BASS_ChannelBytes2Seconds(channel->id, BASS_ChannelGetLength(channel->id, BASS_POS_BYTE));

	Data::SaveData a = A4kGame::gameInstance->saveData;

	for (auto c : AvgEngine::External::BASS::Channels)
	{
		if (c->name.contains("sfx_"))
		{
			c->SetVolume(a.audioData.sfxVolume);
		}
		else
		{
			c->SetVolume(a.audioData.volume);
		}
	}
}

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

	background = std::make_shared<AvgEngine::Base::Sprite>(0, 0, strPath);

	background->tag = "background";

	// set width and height

	background->transform.w = AvgEngine::Render::Display::width;
	background->transform.h = AvgEngine::Render::Display::height;

	background->transform.a = save->gameplayData.backgroundDim;

	background->zIndex = 1;

	// add
	addObject(background);
}

void Average4k::Screens::Menu::Gameplay::loadPlayfield()
{
	std::string folder = "Assets/Noteskin/" + save->gameplayData.noteskin;

	if (!AvgEngine::Utils::Paths::pathExists(folder)) // check steam
	{
		if (!Helpers::SteamHelper::IsSteamRunning)
		{
			AvgEngine::Logging::writeLog("[Gameplay] [Error] Noteskin folder (" + folder + ") not found. Steam isn't running, but this might be a steam skin. Returning to main menu.");
			return;
		}

		for (auto p : Steam::UGCHandler::Instance->subscribedNoteskins)
		{
			if (p.first == save->gameplayData.noteskin)
			{
				folder = p.second;
				break;
			}
		}

		if (!AvgEngine::Utils::Paths::pathExists(folder))
		{
			AvgEngine::Logging::writeLog("[Gameplay] [Error] Noteskin folder (" + folder + ") not found. Returning to main menu.");
			return;
		}
	}

	std::string png = folder + "/noteskin.png";

	if (!AvgEngine::Utils::Paths::pathExists(png))
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] Noteskin texture (" + png + ") not found, returning to main menu.");
		leave();
		return;
	}

	std::string p = folder + "/skin.lua";

	if (!AvgEngine::Utils::Paths::pathExists(p))
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] Noteskin lua file (" + p + ") not found, returning to main menu.");
		leave();
		return;
	}

	lua = new Average4k::Api::AvgLuaFile(p);
	lua->path = p;

	Average4k::Api::Stubs::LuaMenu::Register(lua->getState());
	Average4k::Api::Stubs::LuaNote::Register(lua->getState());

	Average4k::Api::Stubs::LuaMenu cm = Average4k::Api::Stubs::LuaMenu(std::dynamic_pointer_cast<Average4k::Screens::Menu::Gameplay>(Average4k::A4kGame::Instance->CurrentMenu));

	lua->getState().set("currentMenu", cm);

	auto setup = lua->getState().get<sol::optional<sol::protected_function>>("setup");

	lua->getState().set_function("setNoteSize", Average4k::Api::Functions::FGameplay::SetNoteSize);
	lua->getState().set_function("rotateReceptors", Average4k::Api::Functions::FGameplay::RotateReceptors);

	lua->getState().set_function("getWidthScale", Average4k::Api::Functions::FGame::GetWidthScale);
	lua->getState().set_function("getHeightScale", Average4k::Api::Functions::FGame::GetHeightScale);

	if (setup.has_value())
	{
		sol::protected_function_result result = setup.value()();

		if (!result.valid())
		{
			sol::error err = result;
			AvgEngine::Logging::writeLog("[Lua] Error in setup: " + lua->path + "\n" + err.what());
		}
	}
	else
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No setup function found.");
		leave();
		return;
	}

	_noteskinSheet = AvgEngine::OpenGL::Texture::createWithImage(png);

	_noteskinSheet->dontDelete = true;

	// Lua functions

	setupNote = lua->getState().get<sol::protected_function>("noteSetup");

	if (!setupNote)
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No noteSetup function found.");
		leave();
		return;
	}

	overlayUpdate = lua->getState().get<sol::protected_function>("overlayUpdate");

	if (!overlayUpdate)
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No overlayUpdate function found.");
		leave();
		return;
	}

	hitNote = lua->getState().get<sol::protected_function>("hitNote");

	if (!hitNote)
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No hitNote function found.");
		leave();
		return;
	}

	missNote = lua->getState().get<sol::protected_function>("missNote");

	if (!missNote)
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No missNote function found.");
		leave();
		return;
	}

	receptorHit = lua->getState().get<sol::protected_function>("receptorHit");

	if (!receptorHit)
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No receptorHit function found.");
		leave();
		return;
	}

	receptorRelease = lua->getState().get<sol::protected_function>("receptorRelease");

	if (!receptorRelease)
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No receptorRelease function found.");
		leave();
		return;
	}

	startChart = lua->getState().get<sol::protected_function>("start");

	if (!startChart)
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + lua->path + "\n" + "No start function found.");
		leave();
		return;
	}

	// load receptors

	noteWidth = noteWidth / _noteskinSheet->width; // opengl normalizes the texture coordinates
	noteHeight = noteHeight / _noteskinSheet->height;

	wScale = Average4k::Api::Functions::FGame::GetWidthScale();
	hScale = Average4k::Api::Functions::FGame::GetHeightScale();

	noteScaleW *= wScale;
	noteScaleH *= hScale;

	float startX = (AvgEngine::Render::Display::width / 2) - (((128 * noteSpace) * noteScaleW) * 1.25);

	for (int i = 0; i < 4; i++)
	{
		std::shared_ptr<AvgEngine::Base::Sprite> spr = std::make_shared<AvgEngine::Base::Sprite>(0, 0, _noteskinSheet);
		spr->tag = "_play_receptor_" + std::to_string(i);

		spr->transform.x = ((startX + (((i * (128 * noteSpace))) * noteScaleW)) - (((64 * noteSpace) / 2) * noteScaleW));
		spr->transform.y = 128 * hScale;
		if (downscroll)
			spr->transform.y = AvgEngine::Render::Display::height - (128 * hScale);
		spr->transform.w = (128 * noteScaleW);
		spr->transform.h = (128 * noteScaleH);

		spr->center = true;

		spr->dontDelete = true;

		if (rotateReceptors)
			switch (i)
			{
			case 0:
				spr->transform.angle = 90;
				break;
			case 2:
				spr->transform.angle = 180;
				break;
			case 3:
				spr->transform.angle = -90;
				break;
			}

		addObject(spr);

		Average4k::Api::Stubs::LuaSprite lspr = Average4k::Api::Stubs::LuaSprite(spr);

		auto f = lua->getState().get<sol::optional<sol::protected_function>>("receptorSetup");
		if (f.has_value())
		{
			sol::protected_function_result result = f.value()(lspr, noteWidth, noteHeight);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function receptorSetup.\n" + std::string(err.what()));
			}
		}

		receptors.push_back(spr);
	}
}

void Average4k::Screens::Menu::Gameplay::start()
{
	musicHasPlayed = false;
	currentTime = -1;
	hasStarted = true;
	cNotes = chart.difficulties[_diff].notes;
	holds[0] = nullptr;
	holds[1] = nullptr;
	holds[2] = nullptr;
	holds[3] = nullptr;

	// reset timing cache
	chart.cache = {};
	chart.cacheIndex = 0;

	start_time = std::chrono::steady_clock::now();
	fakeBeat = 0;
	currentBeat = -1;

	// reset stats

	combo = 0;
	hitNotes = 0;
	totalNotes = 0;

	botplay = false;

	// set rate

	channel->RateChange(1);

	// start chart

	sol::protected_function_result result = startChart();

	if (!result.valid())
	{
		sol::error err = result;
		AvgEngine::Logging::writeLog("[Lua] Error in function start.\n" + std::string(err.what()));
	}
}

void Average4k::Screens::Menu::Gameplay::load()
{
	// Judgement

	Average4k::Helpers::JudgementHelper::Clear();

	Average4k::Helpers::JudgementHelper::AddJudgement(180.0f, "Bad");
	Average4k::Helpers::JudgementHelper::AddJudgement(135.0f, "Good");
	Average4k::Helpers::JudgementHelper::AddJudgement(90.0f, "Great");
	Average4k::Helpers::JudgementHelper::AddJudgement(45.0f, "Perfect");
	Average4k::Helpers::JudgementHelper::AddJudgement(22.5f, "Marvelous");

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

	noteSpace = save->gameplayData.noteSpace;
	downscroll = save->gameplayData.downscroll;

	loadChart();
	if (!chart.isValid)
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] Chart is not valid, returning to main menu.");
		leave();
		return;
	}

	if (chart.difficulties.size() == 0)
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] No difficulties found, returning to main menu.");
		leave();
		return;
	}

	AvgEngine::Logging::writeLog("[Gameplay] Playing " + std::string(chart.metadata.title.begin(), chart.metadata.title.end()) + " on " + std::string(chart.difficulties[_diff].name.begin(), chart.difficulties[_diff].name.end()));

	loadAudio();

	if (!channel)
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] Audio not found, returning to main menu.");
		leave();
		return;
	}

	loadBackground();

	loadPlayfield();

	// event handlers

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
			if (e.data == save->keybindData.keyPause)
				leave();

			if (e.data == save->keybindData.keyRestart)
				restart();

			if (!botplay)
			{
				if (e.data == save->keybindData.key0)
					noteHit(0);
				if (e.data == save->keybindData.key1)
					noteHit(1);
				if (e.data == save->keybindData.key2)
					noteHit(2);
				if (e.data == save->keybindData.key3)
					noteHit(3);
			}

			if (e.data == GLFW_KEY_F1)
				botplay = !botplay;

			if (e.data == GLFW_KEY_F10)
				channel->RateChange(channel->rate + 0.25f);
			
			if (e.data == GLFW_KEY_F9)
				channel->RateChange(channel->rate - 0.25f);

			if (e.data == GLFW_KEY_F5)
			{
				if (musicHasPlayed)
				{
					if (channel->isPlaying)
						channel->Stop();
					else
						channel->Play(false);
				}
			}
		});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyRelease, [&](AvgEngine::Events::Event e) {
		if (e.data == save->keybindData.key0)
			noteRelease(0);
		if (e.data == save->keybindData.key1)
			noteRelease(1);
		if (e.data == save->keybindData.key2)
			noteRelease(2);
		if (e.data == save->keybindData.key3)
			noteRelease(3);
	});

	hud = std::make_shared<Average4k::Objects::RenderTexture>(&camera, AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);

	hud_spr = std::make_shared<AvgEngine::Base::Sprite>(0, 0, hud->texture);
	hud_spr->tag = "hud";
	hud_spr->src.h = -1;
	hud_spr->src.y = 1;
	hud_spr->zIndex = 3;

	playfield = std::make_shared<Average4k::Objects::RenderTexture>(&camera, AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);

	playfield_spr = std::make_shared<AvgEngine::Base::Sprite>(0, 0, playfield->texture);

	playfield_spr->tag = "_playfield";
	playfield_spr->src.h = -1;
	playfield_spr->src.y = 1;
	playfield_spr->zIndex = 2;

	addObject(hud_spr);
	addObject(playfield_spr);

	start();

}

void Average4k::Screens::Menu::Gameplay::draw()
{
	if (channel == NULL || stop)
		return;

	if (endAfterSecond)
	{
		std::chrono::duration diff = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - endSecond);

		float diffC = diff.count();

		if (diffC >= 2)
		{
			leave();
			return;
		}
	}

	if (channel->isPlaying)
	{
		currentTime = channel->GetPos();
		currentBeat = chart.GetBeatFromTime(currentTime);
	}
	else if (!musicHasPlayed)
	{
		std::chrono::duration diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time);

		Average4k::Data::Chart::TimingPoint tp = chart.timingPoints[0];

		float secondInABeat = tp.startBeat + (2 * (tp.bpm / 60.0f));

		float t = (float)(diff.count()) / 1000.0f;

		fakeBeat = tp.startBeat + (t * (tp.bpm / 60.0f));

		float p = fakeBeat / secondInABeat;

		float sBeat = -secondInABeat;

		currentBeat = std::lerp(sBeat, 0, p);
		currentTime = currentBeat / (tp.bpm / 60.0f);

		if (currentBeat >= 0)
		{
			musicHasPlayed = true;
			channel->Play();
		}
	}


	if (hasStarted)
	{
		static bool oFps = false;

		oFps = !oFps;

		if (oFps)
			spawnNotes();

		updateNotes();
	}

	// draw hud
	hud->Bind();

	for (auto&& o : GameObjects)
	{
		if (o->tag == "background" || o->tag == "hud" || o->tag.starts_with("_play"))
			continue;
		o->draw();
	}

	hud->Unbind();

	// draw playfield

	playfield->Bind();

	for (auto&& o : GameObjects)
	{
		if (!o->tag.starts_with("_play") || o->tag == "_playfield")
			continue;
		o->draw();
	}

	playfield->Unbind();

	if (background != NULL)
	{
		// draw bg
		background->draw();
	}

	// draw render textures
	playfield_spr->draw();
	hud_spr->draw();

	if (!endAfterSecond)
		if ((currentTime >= channel->length) || (notes.size() == 0 && cNotes.size() == 0))
		{
			endAfterSecond = true;
			endSecond = std::chrono::steady_clock::now();
			return;
		}

	A4kGame::gameInstance->DrawOutlinedDebugText(24, AvgEngine::Render::Display::height - 42, 
		"Beat: " + std::to_string(currentBeat) + 
		" | Time: " + std::to_string(currentTime) + 
		" | FPS: " + std::to_string((int)(std::floorf(A4kGame::Instance->fps))) + 
		"/" + std::to_string((int)(std::floorf(A4kGame::Instance->fpsCap))) + 
		" | Scroll Modifier: " + std::to_string(scrollModifier) + 
		" | Stretch: " + std::to_string(stretch), 42);

	A4kGame::gameInstance->DrawOutlinedDebugText(24, AvgEngine::Render::Display::height - 84, "Notes: " + std::to_string(notes.size()) + "/" + std::to_string(cNotes.size()) + " (queued)", 42);

	// botplay

	if (botplay)
		A4kGame::gameInstance->DrawOutlinedDebugText(24, AvgEngine::Render::Display::height - 126, "Botplay enabled.", 42);
}

void Average4k::Screens::Menu::Gameplay::spawnNotes()
{
	if (cNotes.size() == 0)
		return;

	// check for tails

	int i = 0;
	for (auto n : cNotes)
	{
		if (n.type == 3) // skip ends
		{
			if (notes.size() != 0 && holds[n.lane] != nullptr)
			{
				std::shared_ptr<Average4k::Objects::BaseNote> last = holds[n.lane];
				last->endBeat = n.beat;
				last->endTime = chart.GetTimeFromBeat(n.beat);
				holds[n.lane] = nullptr;

				cNotes.erase(cNotes.begin() + i);

				if (cNotes.size() == 0)
					return;
				break;
			}
		}
		i++;
	}

	float cmod = (save->gameplayData.constantMod / channel->rate) * hScale;
	float xmod = save->gameplayData.multiplierMod * hScale;

	Average4k::Data::Chart::Note n = cNotes[0];

	float add = (6.0f * xmod);

	bool spawn = n.beat < (currentBeat + add);

	if (spawn && n.type != 3)
	{


		std::shared_ptr<Average4k::Objects::BaseNote> no = nullptr;

		switch (n.type)
		{
		case 1: // hold head
			no = std::make_shared<Average4k::Objects::HoldNote>(_noteskinSheet, n, &chart, false, cmod, xmod);
			holds[n.lane] = no;
			break;
		default:
			no = std::make_shared<Average4k::Objects::BaseNote>(_noteskinSheet, n, &chart, false, cmod, xmod);
			break;
		}

		no->useXmod = !save->gameplayData.useCmod;
		no->downscroll = downscroll;

		no->tag = "_play_note_" + std::to_string(n.beat);
		no->noteTime = chart.GetTimeFromBeat(n.beat);

		no->dontDelete = true;

		Average4k::Api::Stubs::LuaNote lspr = Average4k::Api::Stubs::LuaNote(no);

		sol::protected_function_result result = setupNote(lspr, noteWidth, noteHeight, n.beat, n.type, n.lane);

		if (!result.valid())
		{
			sol::error err = result;
			AvgEngine::Logging::writeLog("[Lua] [Warning] Error in function noteSetup for note type " + std::to_string(n.type) + " at beat " + std::to_string(n.beat) + ".\n" + std::string(err.what()));
		}

		no->transform.w = (128 * noteScaleW);
		no->transform.h = (128 * noteScaleH);
		no->center = true;

		notes.push_back(no);

		addObject(no);

		cNotes.erase(cNotes.begin());

		if (cNotes.size() == 0)
			return;

		n = cNotes[0];

		if (n.beat < currentBeat + 6)
			spawnNotes();
	}
}

void Average4k::Screens::Menu::Gameplay::updateNotes()
{

	std::vector<std::shared_ptr<Average4k::Objects::BaseNote>> toRemove;

	// scroll modifiers

	Average4k::Data::Chart::ScrollPoint sp = chart.GetScrollPoint(currentBeat);

	scrollModifier = sp.scrollMultiplier;

	Average4k::Data::Chart::SpeedPoint spd = chart.GetSpeedPoint(currentBeat);

	if (spd.spdId != 0)
		_lastStretch = chart.speedPoints[spd.spdId - 1].stretch;

	if (spd.stretch >= 0)
	{
		float progress = 0;

		if (spd.type == 0)
			progress = (currentBeat - spd.startBeat) / (spd.endBeat - spd.startBeat);
		else
			progress = (currentTime - spd.startTimestamp) / (spd.endTimestamp - spd.startTimestamp);


		stretch = std::lerp(_lastStretch, spd.stretch, std::min(progress, 1.0f));

		if (progress <= 1)
			A4kGame::gameInstance->DrawOutlinedDebugText(24, AvgEngine::Render::Display::height - 84, "Progress on stretch: " + std::to_string((int)std::floorf(progress * 100)) + "%", 42);
	}

	for (auto n : notes)
	{
		int lane = n->data.lane;

		if (holdingLanes[lane] == 1 && n->data.type == Data::Chart::Mine && !n->hit)
		{
			std::string judge = Average4k::Helpers::JudgementHelper::GetJudgement(std::abs(currentTime - n->noteTime) * 1.5);

			if (judge == "Marvelous")
			{
				n->hit = true;
				hitNotes -= 1;
				
				float acc = (float)hitNotes / (float)totalNotes;

				if (totalNotes == 0)
					acc = 0;

				sol::protected_function_result result = hitNote(n->data.type, n->data.lane, "Mine", combo, acc);

				if (!result.valid())
				{
					sol::error err = result;
					AvgEngine::Logging::writeLog("[Lua] [Warning] Error in function hitNote for note type " + std::to_string(n->data.type) + " at beat " + std::to_string(n->data.beat) + ".\n" + std::string(err.what()));
				}

				continue;
			}
		}
			

		if (n->overlays.size() != 0)
		{

			sol::protected_function_result result = overlayUpdate(n->data.type, n->overlayAngle, n->overlayOpacity);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] [Warning] Overlay for " + std::to_string(n->data.type) + " errored out.\n" + std::string(err.what()));
			}
			else
			{
				sol::table t = result.get<sol::table>();
				sol::type ty = result.get_type();
				if (!t.valid())
				{
					AvgEngine::Logging::writeLog("[Lua] [Warning] Overlay for " + std::to_string(n->data.type) + " returned a " + t.as<std::string>() + " instead of a sol::table.");
				}
				else
				{
					if (t.size() != 2)
					{
						AvgEngine::Logging::writeLog("[Lua] [Warning] Overlay for " + std::to_string(n->data.type) + " returned an invalid table.");
					}
					else
					{
						n->overlayAngle = t[1];
						n->overlayOpacity = t[2];
					}
				}
			}
		}

		n->stretch = stretch;

		n->currentBeat = currentBeat;
		n->currentTime = currentTime;

		if (botplay && (n->data.type == Data::Chart::Tap || n->data.type == Data::Chart::Head))
		{
			if (!n->hit)
			{
				float d = n->noteTime - currentTime;
				if (d <= 0.01)
				{
					noteHit(lane);

					if (n->data.type == Data::Chart::Head)
					{
						std::shared_ptr<Average4k::Objects::HoldNote> hn = std::dynamic_pointer_cast<Average4k::Objects::HoldNote>(n);
						hn->holding = true;
						n->hit = true;
					}
					else
						noteRelease(lane);
				}
			}
			else
			{
				if (n->data.type == Data::Chart::Head)
				{
					std::shared_ptr<Average4k::Objects::HoldNote> hn = std::dynamic_pointer_cast<Average4k::Objects::HoldNote>(n);
					if (hn->holding)
					{
						float d = std::abs(hn->endTime - currentTime);
						if (d <= 0.01)
							noteRelease(lane);
					}
				}

			}

		}


		if (n->noteTime - currentTime < 0 && !n->missed && n->data.type != Data::Chart::Fake && n->data.type != Data::Chart::Mine)
		{
			bool noMiss = false;
			if (n->hit)
			{
				// check if the note is a hold head

				if (n->data.type == Data::Chart::Head)
				{
					std::shared_ptr<Average4k::Objects::HoldNote> hn = std::dynamic_pointer_cast<Average4k::Objects::HoldNote>(n);

					if (hn->holdHasEnded)
						noMiss = true;
					else
					{
						if (hn->holding)
							noMiss = true;
						else
						{
							float diff = std::abs(hn->endHold - currentBeat);

							if (diff <= 0.25) // grab back
								noMiss = true;

							float endDiff = hn->endTime - currentTime;

							if (endDiff <= 0.25)
								noMiss = true;
						}
					}
				}
				else
					noMiss = true;
			}

			if (!noMiss)
			{
				std::string judgement = Average4k::Helpers::JudgementHelper::GetJudgement(currentTime - n->noteTime);

				if (judgement == "Miss")
				{
					n->missed = true;
					noteMiss();
				}
			}
		}

		if (n->useXmod)
			n->xmod = save->gameplayData.multiplierMod * scrollModifier;

		n->cmod = (save->gameplayData.constantMod / channel->rate) * hScale;

		if (!save->gameplayData.useCmod)
		{
			float diff = n->endBeat - n->data.beat;

			if (n->endBeat <= 0.001f || n->endTime <= 0.001f)
				diff = 0;

			if (n->data.beat + diff < currentBeat - 8)
			{
				toRemove.push_back(n);
				continue;
			}
		}
		else
		{
			float t = n->noteTime;

			float diff = n->endTime - t;

			if (n->endBeat <= 0.001f || n->endTime <= 0.001f)
				diff = 0;

			if (t + diff < currentTime - 8)
			{
				toRemove.push_back(n);
				continue;
			}
		}

		n->transform.y = receptors[lane]->transform.y;
		n->transform.x = receptors[lane]->transform.x;

	}

	for (auto&& n : toRemove)
	{
		notes.erase(std::remove(notes.begin(), notes.end(), n), notes.end());
		removeObject(n);
	}
	toRemove.clear();
}
