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

		GameplayLuaFile* file;


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

#ifdef _DEBUG
		void RunLua(std::string luaCode)
		{
			auto result = file->lua->safe_script(luaCode);
			if (!result.valid())
			{
				sol::error error = result;
				AvgEngine::Logging::writeLog("[Lua] [Error] Lua error!\n" + std::string(error.what()));
			}
		}
#endif

		void load()	override;


	};
}