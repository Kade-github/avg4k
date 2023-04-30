#pragma once
#include "includes.h"
#include "LuaFile.h"
#include "LuaText.h"
namespace Average4k::Lua
{
	class MenuLuaFile : public LuaFile
	{
	public:


		MenuLuaFile(std::string path) : LuaFile(path)
		{
			objects = {};
		}


		~MenuLuaFile()
		{
			objects.clear();
			if (lua != NULL)
			{
				sol::state* l = lua.release();
				delete l;
			}
			LuaFile::~LuaFile();
		}

		void SetPacks(sol::global_table t);


		void Load() override;
	};
}