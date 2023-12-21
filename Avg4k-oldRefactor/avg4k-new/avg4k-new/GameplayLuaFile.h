#pragma once
#include "includes.h"
#include "LuaFile.h"
#include "LuaText.h"
namespace Average4k::Lua
{
	class GameplayLuaFile : public LuaFile
	{
	public:
		std::unordered_map<int, Base::gameObject> setObjects;

		GameplayLuaFile(std::string path) : LuaFile(path)
		{
			objects = {};
			for (int i = 0; i < 10; i++)
				setObjects.insert(std::make_pair(i, Base::gameObject()));
		}


		~GameplayLuaFile()
		{
			objects.clear();
			if (lua != NULL)
			{
				sol::state* l = lua.release();
				delete l;
			}
			LuaFile::~LuaFile();
		}

		void Launch() override;

		void Load() override;
	};
}