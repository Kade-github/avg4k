#pragma once
#include "includes.h"
#include "LuaRect.h"
#include "Display.h"
namespace Average4k::Lua::Base
{
	struct gameObject {
	public:
		int id = 0;
		rect transform{};
		bool isTop = false;
		gameObject* parrent;
		std::vector<gameObject> children;

		void add(gameObject o)
		{
			children.push_back(o);
		}

		void removeObject(int id)
		{
			children.erase(std::ranges::remove_if(children,
				[&](const gameObject x) { return x.id == id; }).begin(), children.end());
		}

	};
}