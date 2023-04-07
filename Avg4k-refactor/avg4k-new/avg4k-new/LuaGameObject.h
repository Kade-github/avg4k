#pragma once
#include "includes.h"
#include "LuaRect.h"
#include "GameObject.h"
namespace Average4k::Lua::Base
{
	struct gameObject {
	public:
		AvgEngine::Base::GameObject* base = NULL;
		rect transform{};
		gameObject* parent = NULL;
		std::vector<gameObject> children;

		int id = 0;
		int type = 0;

		virtual ~gameObject() {}

		int getZIndex()
		{
			if (base)
				return base->transformRatio;
			return false;
		}

		void setZIndex(int z)
		{
			if (base)
				base->zIndex = z;
		}

		bool getRatio()
		{
			if (base)
				return base->transformRatio;
			return false;
		}

		void setRatio(bool ratio)
		{
			if (base)
				base->setRatio(ratio);
		}

		gameObject() {
			id = 0;
			type = 0;
			parent = NULL;
			children = {};
			transform = {};
			base = NULL;
		}

		gameObject(float x, float y) {
			transform.x = x;
			transform.y = y;
			id = 0;
			type = 0;
			parent = NULL;
			children = {};
			transform = {};
			base = NULL;
		}

		void destroy();

		void add(gameObject& o)
		{
			if (!o.base)
			{
				AvgEngine::Logging::writeLog("[Lua] [Error] Failed to add " + std::to_string(o.id) + ", it doesn't exist! (please call create on it before adding it)");
				return;
			}
			if (!base)
			{
				AvgEngine::Logging::writeLog("[Lua] [Error] Failed to add " + std::to_string(o.id) + ", YOU don't exist. (please call create on the object you want stuff to be added to)");
				return;
			}
			base->addObject(o.base);
			o.id = o.base->id;
			children.push_back(o);
		}

		void removeObject(gameObject& o)
		{
			if (!o.base)
			{
				AvgEngine::Logging::writeLog("[Lua] [Error] Failed to remove " + std::to_string(o.id) + ", it doesn't exist!");
				return;
			}
			if (!base)
			{
				AvgEngine::Logging::writeLog("[Lua] [Error] Failed to remove " + std::to_string(o.id) + ", YOU don't exist. (you cant remove something from something that doesn't exist!)");
				return;
			}
			base->removeObject(o.id);
			children.erase(std::ranges::remove_if(children,
				[&](const gameObject x) { return x.id == o.id; }).begin(), children.end());
		}


	};
}