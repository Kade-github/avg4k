#pragma once
#include "Object.h"

class AvgHitGraph : public Object {
public:
	std::map<float, float> dataPoints;

	Rect clipRect;

	float drawX;
	float drawY;

	float yScale = 1;

	AvgHitGraph(int x, int y, int _w, int _h) : Object(x, y)
	{
		w = _w;
		h = _h;

	}

	void mouseWheel(float amount)
	{
		int mx, my;
		Game::GetMousePos(&mx, &my);

		bool one = (mx < drawX + w && mx > drawX);
		bool two = (my < drawY + h && my > drawY);

		if (one && two)
		{
			yScale += amount;
		}
	}

	void draw() {
		if (dataPoints.size() == 0)
			return;

		drawX = x;
		drawY = y;

		clipRect.x = x;
		clipRect.y = y;
		clipRect.w = w;
		clipRect.h = h;

		if (parent != NULL)
		{
			AvgContainer* c = (AvgContainer*)parent;
			if (clipRect.x < (c->clipRect.x))
				clipRect.x = c->clipRect.x;
			if (clipRect.y < (c->clipRect.y))
				clipRect.y = c->clipRect.y;

			if (clipRect.x > (c->clipRect.x + c->w))
				clipRect.x = c->clipRect.x;
			if (clipRect.y > (c->clipRect.y + c->h))
				clipRect.y = c->clipRect.y;

			if (clipRect.x + clipRect.w > c->x + c->w)
				clipRect.w = (c->x + c->w) - (clipRect.x + clipRect.w);
			if (clipRect.y + clipRect.h > c->y + c->h)
				clipRect.h = (c->y + c->y) - (clipRect.y + clipRect.h);
		}


		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(&clipRect);


		Rendering::drawBatch();

		Rect src = { 0,0,1,1 };

		Rect r = { x,y,6,h };

		Rendering::PushQuad(&r, &src, NULL, GL::genShader);

		r = { x,y + h - 6.0f,(float)w,6.0f };

		Rendering::PushQuad(&r, &src, NULL, GL::genShader);

		Rendering::drawBatch();
		float end = (--dataPoints.end())->first;
		float latest = (Judge::hitWindows[4] * 2);

		float hh = (h - 12);

		// judgement lines


		for (float f : Judge::hitWindows)
		{
			float centered = f + Judge::hitWindows[4];

			float yy = y + ((hh - 12) * (centered / latest));

			r = { x + 6,yy,(float)w - 6,4};

			judgement j = Judge::judgeNote(f);

			switch (j)
			{
			case judgement::Judge_marvelous:
				r.r = 50;
				r.g = 255;
				r.b = 238;
				break;
			case judgement::Judge_perfect:
				r.r = 255;
				r.g = 231;
				r.b = 50;
				break;
			case judgement::Judge_great:
				r.r = 88;
				r.g = 255;
				r.b = 77;
				break;
			case judgement::Judge_good:
				r.r = 181;
				r.g = 255;
				r.b = 84;
				break;
			case judgement::Judge_bad:
				r.r = 207;
				r.g = 53;
				r.b = 23;
				break;
			case judgement::Judge_miss:
				r.r = 255;
				r.g = 0;
				r.b = 0;
				break;
			}

			r.a = 0.25;

			Rendering::PushQuad(&r, &src, NULL, GL::genShader);
			Rendering::drawBatch();
			yy = (y + ((hh - 12) / 2)) - ((hh - 12) * (f / latest));

			r.y = yy;

			Rendering::PushQuad(&r, &src, NULL, GL::genShader);
			Rendering::drawBatch();
		}

		for (auto p : dataPoints)
		{
			float xx = x + 6 + (((w - 6)) * (p.first / end));
			float yy = y + ((hh - 12) * ((p.second + Judge::hitWindows[4]) / latest));

			judgement j = Judge::judgeNote(p.second);

			r = { xx,yy,4,4 };

			switch (j)
			{
			case judgement::Judge_marvelous:
				r.r = 50;
				r.g = 255;
				r.b = 238;
				break;
			case judgement::Judge_perfect:
				r.r = 255;
				r.g = 231;
				r.b = 50;
				break;
			case judgement::Judge_great:
				r.r = 88;
				r.g = 255;
				r.b = 77;
				break;
			case judgement::Judge_good:
				r.r = 181;
				r.g = 255;
				r.b = 84;
				break;
			case judgement::Judge_bad:
				r.r = 207;
				r.g = 53;
				r.b = 23;
				break;
			case judgement::Judge_miss:
				r.r = 255;
				r.g = 0;
				r.b = 0;
				break;
			}

			Rendering::PushQuad(&r, &src, NULL, GL::genShader);
			Rendering::drawBatch();
		}


		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(NULL);
	}
};