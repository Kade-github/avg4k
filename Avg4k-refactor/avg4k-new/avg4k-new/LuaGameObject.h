#pragma once
#include "includes.h"
#include "LuaRect.h"
#include "GameObject.h"
namespace Average4k::Lua::Base
{
	struct gameObject {
	public:
		AvgEngine::Base::GameObject* base;
		rect transform{};
		gameObject* parent = NULL;
		std::vector<gameObject> children;

		virtual ~gameObject() {}
		int id = 0;
		int type = 0;

		bool getRatio()
		{
			if (base)
				return base->transformRatio;
			return false;
		}

		void setRatio(bool ratio)
		{
			if (base)
				base->transformRatio = ratio;
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
			base->addObject(o.base);
			children.push_back(o);
		}

		void removeObject(gameObject& o)
		{
			base->removeObject(o.id);
			children.erase(std::ranges::remove_if(children,
				[&](const gameObject x) { return x.id == o.id; }).begin(), children.end());
		}


	};
}