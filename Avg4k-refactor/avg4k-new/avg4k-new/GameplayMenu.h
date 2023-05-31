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

		int combo = 0;
		int highestCombo = 0;

		bool botplay = false;
		bool scoreSubmittable = true;

		GameplayLuaFile* file;

		void UpdateAccuracy(int judge)
		{
			switch (judge)
			{
			case 6:
			case 0:
				notesHit += 1;
				break;
			case 1:
				notesHit += 0.925f;
				break;
			case 2:
				notesHit += 0.7f;
				break;
			case 3:
				notesHit += 0.35f;
				break;
			case 4:
				notesHit += 0.1f;
				break;
			}
			if (judge != 5)
				combo++;
			else
				combo = 0;

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
			file->Function("Update", std::to_string(glfwGetTime()));
			Menu::draw();
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