#pragma once
#include "includes.h"
#include "LuaRect.h"
#include "Display.h"
namespace Average4k::Lua::Base
{
	struct gameObject {
	public:
		rect transform{};
		bool isTop = false;
		gameObject* parrent = NULL;
		std::vector<gameObject> children;

		int id = 0;
		int type = 0;

		gameObject() = default;

		void add(gameObject o)
		{
			children.push_back(o);
		}

		void removeObject(int id)
		{
			children.erase(std::ranges::remove_if(children,
				[&](const gameObject x) { return x.id == id; }).begin(), children.end());
		}

		gameObject(float x, float y) {
			transform.x = x;
			transform.y = y;
		}

	};
}