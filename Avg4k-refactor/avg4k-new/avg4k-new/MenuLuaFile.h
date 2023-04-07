#pragma once
#include "includes.h"
#include "LuaFile.h"
#include "LuaText.h"
namespace Average4k::Lua
{
	class MenuLuaFile : public LuaFile
	{
	public:

		std::vector<Average4k::Lua::Base::gameObject> objects{};

		MenuLuaFile(std::string path) : LuaFile(path)
		{
			objects = {};
		}


		~MenuLuaFile()
		{
			objects.clear();
			sol::state* l = lua.release();
			delete l;
		}

		void SetPacks(sol::global_table t);


		void CreateObject(Average4k::Lua::Base::gameObject& ob);

		void AddObject(Average4k::Lua::Base::gameObject& ob);

		void RemoveObject(Average4k::Lua::Base::gameObject& ob);

		void Load() override;
	};
}