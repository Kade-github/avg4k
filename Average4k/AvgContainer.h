#pragma once
#include "Object.h"
#include "Texture.h"
#include "Rendering.h"
#include "Game.h"

struct itemId {
	std::string name;
	Object* obj;
	bool below;
};

class AvgContainer : public Object
{
public:
	Rect clipRect;
	Texture* tex;
	float scrollProg = 0;
	float scrollAddition = 0;
	float maxScroll = 0;

	bool active = true;

	std::vector<itemId> items;

	std::vector<Object*> above;
	std::vector<Object*> below;
	bool dropDownActive = false;
	Texture* scrollAr;
	Rect scrollBar;

	AvgContainer(int _x, int _y, Texture* background)
	{
		MUTATE_START
		x = _x;
		y = _y;
		tex = background;
		w = tex->width;
		h = tex->height;
		customDraw = true;
		scrollAr = Noteskin::getMenuElement(Game::noteskin, "Container/scroll_arrow.png");
		MUTATE_END
	}

	~AvgContainer()
	{
		for (Object* obj : below)
			delete obj;
		for (Object* obj : above)
			delete obj;
		items.clear();
		below.clear();
		above.clear();
	}
	
	Object* findItemByName(std::string name)
	{
		for (itemId i : items)
			if (i.name == name)
				return i.obj;
	}

	Object* addObject(Object* obj, std::string name, bool beloww = false)
	{
		if (!beloww)
			above.push_back(obj);
		else
			below.push_back(obj);

		itemId id;
		id.name = name;
		id.below = beloww;
		id.obj = obj;
		obj->parent = this;
		obj->create();
		items.push_back(id);
		return obj;
	}

	void removeObject(Object* obj, bool beloww = false)
	{
		if (!beloww)
			above.erase(
				std::remove_if(above.begin(), above.end(), [&](Object* const ob) {
					return ob->id == obj->id;
					}),
				above.end());
		else
			below.erase(
				std::remove_if(below.begin(), below.end(), [&](Object* const ob) {
					return ob->id == obj->id;
					}),
				below.end());
		items.erase(
			std::remove_if(items.begin(), items.end(), [&](itemId const ob) {
				return ob.obj->id == obj->id;
				}),
			items.end());
	}

	void draw();


	void textInput(SDL_TextInputEvent event)
	{
		if (!active)
			return;
		for (Object* obj : above)
		{
			obj->textInput(event);
		}
	}

	void mouseDown()
	{
		if (!active)
			return;
		for (Object* obj : above)
		{
			obj->mouseDown();
		}
	}

	void keyDown(SDL_KeyboardEvent ev)
	{
		if (!active)
			return;
		for (Object* obj : above)
		{
			obj->keyDown(ev);
		}
	}

	void mouseWheel(float amount)
	{
		if (!active)
			return;
		int mx, my;
		Game::GetMousePos(&mx, &my);

		my -= y;
		mx -= x;

		int pH = h;

		if (parent != NULL)
			pH = parent->h;

		bool one = (mx < w);
		bool two = (my < pH * 1.3);
		if (one && two && maxScroll > 0)
		{
			float max = (h - 15) - scrollAddition;
			scrollAddition += -(amount * 40);
			if (scrollAddition > maxScroll)
				scrollAddition = maxScroll;
			if (scrollAddition < 0)
				scrollAddition = 0;
		}

		for (Object* obj : above)
		{
			obj->mouseWheel(amount);
		}
	}
};