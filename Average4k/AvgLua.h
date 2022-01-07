#pragma once
#include "pch.h"
#include "luaaa.h"
#include "Menu.h"
#include "Game.h"
#include "AvgSprite.h"
#include "Text.h"
using namespace luaaa;
namespace Lua
{
	class AvgLuaSprite : public AvgSprite {
	public:
		AvgLuaSprite(int x, int y, std::string path) : AvgSprite(x, y, path)
		{
			
		}
		void destroy()
		{
			beforeDeath();
			die();
		}
	};
	class AvgText : public Text {
	public:

	};
	class AvgMenu : public Menu {
		public:
			void switchTo(bool trans) {
				if (trans)
					Game::instance->transitionToMenu(this);
				else
					Game::instance->switchMenu(this);
			}

	};
	class AvgLua {
	public:
		static void getMenu(std::string filePath)
		{

		}
		static void exportShit()
		{

			
		}
	};
}