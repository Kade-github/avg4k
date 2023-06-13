#include "Gameplay.h"
#include "Average4K.h"
#include "TimeUtils.h"
#include "MainMenu.h"
using namespace AvgEngine;

namespace Average4ker
{
	Average4K* a4er = NULL;
}


void Gameplay::End()
{
	c->Stop();
	Average4ker::a4er->SwitchMenu(new MainMenu(Average4K::skin->GetLua("main-menu")));
	ended = true;
}

void Gameplay::AddPlayfield()
{
	float noteSize = std::stof(Average4K::settings->Get("Note Size").value);
	float xPos = Average4K::Instance->CurrentMenu->displayRect.w / 2 - (((((64 * noteSize)) * 4) - (64 * noteSize)) / 2);
	float yPos = (64 * noteSize);
	Average4k::Objects::Gameplay::Playfield* playfield = new Average4k::Objects::Gameplay::Playfield(xPos, yPos, &Average4ker::a4er->options.currentFile->chartMetadata.Difficulties[Average4ker::a4er->options.currentFile_diff]);

	addObject(playfield);

	playfields.push_back(playfield);

	playfield->tag = "playfield-" + std::to_string(playfields.size());
}

void Gameplay::Reload()
{
	Logging::writeLog("[Gameplay] You cannot reload this menu!");
}

void Gameplay::load()
{
	Average4ker::a4er = static_cast<Average4K*>(Average4K::Instance);
	Average4k::Lua::GameplayMenu::load();
	Average4ker::a4er->options.inGameplay = true;
	// Create playfield player #1
	AddPlayfield();

	using namespace Average4k::Audio;

	c = RhythmBASSHelper::GetChannel("menu");
	if (c == NULL)
	{
		Logging::writeLog("[Error] Rhythm Channel 'menu' doesn't exist! Did lua create it?");
		return;
	}
	c->Stop();
	c->SetPos(0);
	startTimestamp = glfwGetTime();
	songStart = startTimestamp + std::stof(Average4K::settings->Get("Start Offset").value);


	// Key subscribes
	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
		if (!botplay)
			for (Average4k::Objects::Gameplay::Playfield* p : playfields)
				p->keyPress(e.data);

		if (e.data == GLFW_KEY_ESCAPE)
		{
			End();
		}

		if (e.data == GLFW_KEY_F1)
		{
			botplay = !botplay;
			scoreSubmittable = false;
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyRelease, [&](AvgEngine::Events::Event e) {
		if (!botplay)
			for (Average4k::Objects::Gameplay::Playfield* p : playfields)
				p->keyRelease(e.data);
	});
	menuStartimestamp = glfwGetTime();
}


void Gameplay::draw()
{
	if (ended)
	{
		Average4k::Lua::GameplayMenu::draw();
		return;
	}
	for (QueuedPacket& p : Average4ker::a4er->queuedPackets)
	{

	}


	if (c == NULL)
	{
		Average4k::Lua::GameplayMenu::draw();
		return;
	}

	// check for the start offset
	if (glfwGetTime() >= songStart && !c->isPlaying)
	{
		c->Play();
		Logging::writeLog("[Gameplay] Song started at " + std::to_string(songStart));
		isStarted = true;
	}

	float time = c->GetPos() + std::stof(Average4K::settings->Get("Note Offset").value);
	if (!c->isPlaying)
		time = -std::abs(songStart - glfwGetTime());
	float beat = Average4ker::a4er->options.currentFile->GetBeatFromTime(time);

	bool shouldEnd = true;

	if(!isStarted) // we still wanna do this, but w/o the checks. (for before the stuff starts)
		for (Average4k::Objects::Gameplay::Playfield* p : playfields)
		{
			p->time = time;
			p->beat = beat;
		}

	if (isStarted)
	{
		if (!c->isPlaying) // the end
			time = INT_MAX;
		for (Average4k::Objects::Gameplay::Playfield* p : playfields)
		{
			p->time = time;
			p->beat = beat;
			bool b = false;
			for (auto r : p->receptors)
			{
				if (r->notes.size() != 0 || r->Children.size() != 0)
				{
					shouldEnd = false;
					b = true;
					break;
				}
			}
			if (b)
				break;
		}
	}
	else
		shouldEnd = false;

	if (shouldEnd && !ended)
	{
		End();
		return;
	}

	Text* t = NULL;
	Sprite* s = NULL;
	std::string acc = std::to_string(accuracy * 100);
	if (!failedCheck)
	{
		for (auto k : file->setObjects)
		{

			switch (k.first)
			{
			case 0: // background
				s = static_cast<Sprite*>(k.second.base);
				if (s == NULL)
				{
					Logging::writeLog("[Error] [Lua] Background is null! Id 0");
					failedCheck = true;
					break;
				}
				break;
			case 1: // accuracy
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Accuracy text is null! Id 1");
					failedCheck = true;
					break;
				}
				t->text = acc.substr(0, std::min(acc.size(), acc.find('.') + 3)) + "%";
				break;
			case 2: // grade
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Grade text is null! Id 2");
					failedCheck = true;
					break;
				}

				if (accuracy >= 1)
					t->text = "S";
				else if (accuracy >= 0.95)
					t->text = "A+";
				else if (accuracy >= 0.90)
					t->text = "A";
				else if (accuracy >= 0.80)
					t->text = "B";
				else if (accuracy >= 0.70)
					t->text = "C";
				else if (accuracy >= 0.60)
					t->text = "D";
				else
					t->text = "F";

				break;
			case 3: // combo
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Combo text is null! Id 3");
					failedCheck = true;
					break;
				}
				t->text = std::to_string(combo) + "x";
				break;
			case 4: // marv combo counter
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Marv combo counter text is null! Id 4");
					failedCheck = true;
					break;
				}
				t->text = "Marvelous: " + std::to_string(marv);
				break;
			case 5: // perf combo counter
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Perfect combo counter text is null! Id 5");
					failedCheck = true;
					break;
				}
				t->text = "Perfect: " + std::to_string(perf);
				break;
			case 6: // great combo counter
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Great combo counter text is null! Id 6");
					failedCheck = true;
					break;
				}
				t->text = "Great: " + std::to_string(great);
				break;
			case 7: // good combo counter
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Good combo counter text is null! Id 7");
					failedCheck = true;
					break;
				}
				t->text = "Good: " + std::to_string(good);
				break;
			case 8: // bad combo counter
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Bad combo counter text is null! Id 8");
					failedCheck = true;
					break;
				}
				t->text = "Bad: " + std::to_string(bad);
				break;
			case 9: // miss combo counter
				t = static_cast<Text*>(k.second.base);
				if (t == NULL)
				{
					Logging::writeLog("[Error] [Lua] Miss combo counter text is null! Id 9");
					failedCheck = true;
					break;
				}
				t->text = "Miss: " + std::to_string(miss);
				break;
			}
		}
		if (failedCheck)
		{
			Logging::writeLog("[Error] [Lua] Not every object was set. The game will no longer update lua objects. Please fix these ^^^");
			return;
		}
	}

	Average4k::Lua::GameplayMenu::draw();
}