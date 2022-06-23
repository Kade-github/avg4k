#include "AvgContainer.h"
#include "AvgDropDown.h"
#include "MainerMenu.h"

void AvgContainer::mouseWheel(float amount)
	{
		if (!active || MainerMenu::lockInput)
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

void AvgContainer::draw() {
	if (!active)
		return;
	clipRect.x = x + 2;
	clipRect.y = y + 1;
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
	bool fail = true;
	for (itemId i : items)
	{
		if (i.below)
		{
			continue;
		}
		if (parent != nullptr && topAlphas)
		{
			i.obj->alpha = alpha;
		}
		i.obj->parent = this;
		if (i.obj->y > h || i.obj->y < 0) // yes!
		{
			scroll = true;
			if (i.obj->y > 0)
			{
				lastItem = i;
				float first = ((i.obj->y + i.obj->h));
				float second = h;
				maxScroll = (first - second) + 42;
				fail = false;
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
	if (tex != nullptr)
		Rendering::PushQuad(&dstRect, &srcRect, tex, GL::genShader, angle);
	if (shouldUseCallback)
	{
		int mx, my;
		Game::GetMousePos(&mx, &my);
		if (mx > realPosX && my > realPosY && mx < realPosX + w && my < realPosY + h)
		{
			dstRect.a = 0.5;
			Rendering::PushQuad(&dstRect, &srcRect, NULL, GL::genShader, angle);
			dstRect.a = alpha;
		}
	}

	if (scroll && !fail)
	{
		Rect topArrow;
		topArrow.x = x + 5;
		topArrow.y = y + 9;
		topArrow.w = 14;
		topArrow.r = 255;
		topArrow.g = 255;
		topArrow.b = 255;
		topArrow.a = alpha;
		topArrow.h = 6;


		Rect border;
		border.x = x;
		border.y = y;
		border.w = 14;
		border.h = h;

		scrollProg = (h - (12 + topArrow.h)) / (((lastItem.obj->y + 42) - scrollAddition) + lastItem.obj->h);

		if (scrollProg >= 1)
			scrollProg = 1;

		scrollBar.x = topArrow.x - 5;
		scrollBar.h = (h - (12 + topArrow.h)) * ((h - (12 + topArrow.h)) / ((lastItem.obj->y + 42) + lastItem.obj->h));
		scrollBar.r = 255;
		scrollBar.g = 255;
		scrollBar.b = 255;
		scrollBar.a = alpha;

		float max = (h - (topArrow.h + 12));

		scrollBar.y = (topArrow.y + ((14) + scrollProg * (max - (12)))) - (scrollBar.h - (topArrow.h));
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
	AvgDropDown* activeDrop = NULL;
	dropDownActive = false;
	for (Object* a : above)
	{
		AvgDropDown* objb = dynamic_cast<AvgDropDown*>(a);
		if (objb != NULL)
		{
			if (objb->isActive)
			{
				activeDrop = objb;
				dropDownActive = true;
				continue;
			}
		}
		if (alpha > 0)
		{
			a->x += x + scrollBar.w;
			a->y += y - scrollAddition;
			a->realPosX = a->x;
			a->realPosY = a->y;
			a->draw();
			a->x -= x + scrollBar.w;
			a->y -= y - scrollAddition;
		}
	}

	if (activeDrop != NULL)
	{
		activeDrop->x += x + scrollBar.w;
		activeDrop->y += y - scrollAddition;
		activeDrop->draw();
		activeDrop->x -= x + scrollBar.w;
		activeDrop->y -= y - scrollAddition;
	}
	if (clipRect.w > 0 || clipRect.h > 0)
		Rendering::SetClipRect(NULL);
}