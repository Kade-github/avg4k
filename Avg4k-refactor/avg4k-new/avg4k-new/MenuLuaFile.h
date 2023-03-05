#pragma once
#include "includes.h"
#include "LuaFile.h"

namespace Average4k::Lua
{
	class MenuLuaFile : public LuaFile
	{
	public:
		std::vector<GameObject*> unspawnedObjects;


		MenuLuaFile(std::string path) : LuaFile(path)
		{

		}

		GameObject* GetUnspawned(int id)
		{
			for (int i = 0; i < unspawnedObjects.size(); i++)
				if (unspawnedObjects[i]->id == id)
					return unspawnedObjects[i];
			return NULL;
		}

		void Reload() override
		{
			for (GameObject* obj : unspawnedObjects)
				delete obj;
			
			unspawnedObjects.clear();

			LuaFile::Reload();
		}

		void Load() override
		{
			using namespace Average4k::Lua::Base;

			lua->set_function("create", [&](int type, std::unordered_set<std::pair<std::string, std::string>> args)
			{
				switch(type)
				{
				case 0: // Regular sprite

					break;
				}
			});

			lua->set_function("tween", [&](std::string tween, gameObject ob, Base::rect endRect) {
				GameObject* o = Average4K::Instance->CurrentMenu->getObject(ob.id);
				if (o == NULL)
				{
					AvgEngine::Logging::writeLog("[Lua] [Error] Failed to tween " + std::to_string(ob.id) + ", it doesn't exist!");
					return;
				}
			});

			lua->set_function("add", [&](gameObject ob) {
				GameObject* o = GetUnspawned(ob.id);
				if (o == NULL)
				{
					AvgEngine::Logging::writeLog("[Lua] [Error] Failed to add " + std::to_string(ob.id) + ", it doesn't exist!");
					return;
				}
			});

			lua->set_function("remove", [&](int id) {

			});
		}
	};
}