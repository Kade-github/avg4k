#pragma once
#include "Object.h"
#include "Texture.h"
#include "Rendering.h"
#include "Game.h"

struct itemId {
	std::string name;
	Object* obj;
};

class AvgContainer : public Object
{
public:
	Rect clipRect;
	Texture* tex;
	float scrollProg = 0;
	float scrollAddition = 0;
	float maxScroll = 0;
	std::vector<itemId> items;

	std::vector<Object*> above;
	std::vector<Object*> below;
	Texture* scrollAr;
	Rect scrollBar;

	AvgContainer(int _x, int _y, Texture* background)
	{
		x = _x;
		y = _y;
		tex = background;
		w = tex->width;
		h = tex->height;
		customDraw = true;
		scrollAr = Noteskin::getMenuElement(Game::noteskin, "Container/scroll_arrow.png");
	}

	Object* findItemByName(std::string name)
	{
		for (itemId i : items)
			if (i.name == name)
				return i.obj;
	}

	void addObject(Object* obj, std::string name, bool beloww = false)
	{
		if (!beloww)
			above.push_back(obj);
		else
			below.push_back(obj);

		itemId id;
		id.name = name;
		id.obj = obj;
		items.push_back(id);
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

	void draw() {

		clipRect.x = x + 2;
		clipRect.y = y + 2;
		clipRect.w = w - 4;
		clipRect.h = h - 4;



		// sprite stuff

		Rect dstRect;
		Rect srcRect;


		float mpx = (w * (1 - scale)) / 2;
		float mpy = (h * (1 - scale)) / 2;
		dstRect.x = x + mpx;
		dstRect.y = y + mpy;
		if (!staticView)
		{
			dstRect.x -= Game::mainView.x;
			dstRect.y -= Game::mainView.y;
		}
		dstRect.w = w * scale;
		dstRect.h = h * scale;
		dstRect.r = 255;
		dstRect.g = 255;
		dstRect.b = 255;
		dstRect.a = alpha;

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(&clipRect);

		// do we need to draw a scrollbar?
		bool scroll = false;
		itemId lastItem;
		lastItem.name = "NOOOOOO";
		for (itemId i : items)
		{
			if (i.obj->y > h || i.obj->y < 0) // yes!
			{
				scroll = true;
				if (i.obj->y > 0)
				{
					lastItem = i;
					float first = ((i.obj->y + i.obj->h));
					float second = h;
					maxScroll = first - second;
				}
			}
		}


		for (Object* b : below)
		{
			b->x += x;
			b->y += y;
			b->draw();
			b->x -= x;
			b->y -= y;
		}

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(NULL);

		Rendering::PushQuad(&dstRect, &srcRect, tex, GL::genShader, angle);


		if (scroll)
		{
			Rect topArrow;
			topArrow.x = x + 5;
			topArrow.y = y + 9;
			topArrow.w = 14;
			topArrow.r = 255;
			topArrow.g = 255;
			topArrow.b = 255;
			topArrow.a = 1;
			topArrow.h = 6;

			Rect border;
			border.x = x;
			border.y = y;
			border.w = 14;
			border.h = h;

			scrollProg = (h - (12 + topArrow.h)) / ((lastItem.obj->y - scrollAddition) + lastItem.obj->h);

			if (scrollProg >= 1)
				scrollProg = 1;

			scrollBar.x = topArrow.x - 5;
			scrollBar.h = (h - (12 + topArrow.h)) * ((h - (12 + topArrow.h)) / (lastItem.obj->y + lastItem.obj->h));
			scrollBar.r = 255;
			scrollBar.g = 255;
			scrollBar.b = 255;
			scrollBar.a = 1;

			float max = (h - (topArrow.h + 12));

			scrollBar.y = (topArrow.y + ((12) + scrollProg * (max - (12)))) - (scrollBar.h - (topArrow.h));
			scrollBar.w = 22;

			Rendering::PushQuad(&topArrow, &srcRect, scrollAr, GL::genShader, angle);

			topArrow.y = (y + (h - 14));

			srcRect.h = -1;

			Rendering::PushQuad(&topArrow, &srcRect, scrollAr, GL::genShader, angle);

			srcRect.h = 1;

			Rendering::PushQuad(&scrollBar, &srcRect, NULL, GL::genShader, angle);
		}

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(&clipRect);

		for (Object* a : above)
		{
			a->x += x + scrollBar.w;
			a->w -= scrollBar.w;
			a->y += y - scrollAddition;
			a->draw();
			a->x -= x + scrollBar.w;
			a->w += scrollBar.w;
			a->y -= y - scrollAddition;
		}

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(NULL);

	}

	void mouseWheel(float amount)
	{
		int mx, my;
		Game::GetMousePos(&mx, &my);

		bool one = (mx > x && mx < x + w);
		bool two = (my > y && my < y + h);
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
			AvgContainer* objb = dynamic_cast<AvgContainer*>(obj);
			if (objb == NULL)
				continue;
			objb->mouseWheel(amount);
		}
	}
};