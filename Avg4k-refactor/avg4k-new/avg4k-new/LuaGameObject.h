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
		void setTransform(rect t)
		{
			transform = t;
			if (base)
			{
				transform.base = &base->transform;
				transform.base->x = t.x;
				transform.base->y = t.y;
				transform.base->w = t.w;
				transform.base->h = t.h;
				transform.base->r = t.r;
				transform.base->g = t.g;
				transform.base->b = t.b;
				transform.base->a = t.a;
				transform.base->scale = t.scale;
				transform.base->angle = t.deg;
			}
		}
		rect& getTransform()
		{
			return transform;
		}
		rect transformOffset{};
		rect clipRect{};
		gameObject* parent = NULL;
		std::vector<gameObject> children;

		int zIndex = 0;

		int id = 0;
		int type = 0;

		virtual ~gameObject() {}

		std::string getTag()
		{
			if (base)
				return base->tag;
			return "object";
		}

		void setTag(std::string tag)
		{
			if (base)
				base->tag = tag;
		}

		gameObject& getChildByTag(std::string tag)
		{
			for (auto& c : children)
			{
				if (c.getTag() == tag)
					return c;
			}
			gameObject o;
			return o;
		}

		int getZIndex()
		{
			return zIndex;
		}

		void setZIndex(int z)
		{
			zIndex = z;
			if (base)
				base->zIndex = z;
		}

		bool getVisible()
		{
			if (base)
				return base->drawn;
			return true;
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

		rect denormilize()
		{
			rect nr{};
			if (base)
				nr = rect(base->iTransform.x, base->iTransform.y, base->iTransform.w, base->iTransform.h, base->iTransform.r, base->iTransform.g, base->iTransform.b, base->iTransform.a, base->iTransform.scale, base->iTransform.angle);
			return nr;
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
			o.parent = this;
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

		void removeAll()
		{
			if (!base)
			{
				AvgEngine::Logging::writeLog("[Lua] [Error] Failed to remove all, YOU don't exist. (you cant remove something from something that doesn't exist!)");
				return;
			}
			base->removeAll();
			children.clear();
		}

	};
}