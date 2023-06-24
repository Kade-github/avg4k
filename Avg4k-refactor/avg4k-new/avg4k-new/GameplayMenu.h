#pragma once
#include "includes.h"
#include "Menu.h"
#include "GameplayLuaFile.h"
namespace Average4k::Lua
{
	class GameplayMenu : public AvgEngine::Base::Menu
	{
	public:
		bool luaMenu = true;

		float notesHit = 0;
		float totalNotes = 0;

		float accuracy = 1;

		bool ended = false;
		bool isStarted = false;

		int combo = 0;
		int highestCombo = 0;

		bool botplay = false;
		bool scoreSubmittable = true;

		int marv = 0;
		int perf = 0;
		int great = 0;
		int good = 0;
		int bad = 0;
		int miss = 0;

		GameplayLuaFile* file;

		void UpdateAccuracy(int judge)
		{
			switch (judge)
			{
			case 6:
			case 0:
				notesHit += 1;
				marv++;
				break;
			case 1:
				notesHit += 0.925f;
				perf++;
				break;
			case 2:
				notesHit += 0.7f;
				great++;
				break;
			case 3:
				notesHit += 0.35f;
				good++;
				break;
			case 4:
				notesHit += 0.1f;
				bad++;
				break;
			default:
				miss++;
				combo = 0;
				break;
			}
			if (judge != 5)
				combo++;

			if (combo > highestCombo)
				highestCombo = combo;

			totalNotes++;
			accuracy = notesHit / totalNotes;
		}

		GameplayMenu() : AvgEngine::Base::Menu()
		{
			file = NULL;

		}

		GameplayMenu(std::string path) : AvgEngine::Base::Menu()
		{
			file = new GameplayLuaFile(path);
			luaMenu = true;
		}

		virtual void Reload()
		{

		}

		~GameplayMenu()
		{
			if (file != NULL)
				delete file;
		}

		void draw() override
		{
			Menu::draw();
			if ((int)(glfwGetTime() * 1000) % 2 == 0)
				file->Function("Update", std::to_string(glfwGetTime()));
		}

		void RunLua(std::string luaCode)
		{
			auto result = file->lua->safe_script(luaCode);
			if (!result.valid())
			{
				sol::error error = result;
				AvgEngine::Logging::writeLog("[Lua] [Error] Lua error!\n" + std::string(error.what()));
			}
		}


		void load()	override;


	};
}