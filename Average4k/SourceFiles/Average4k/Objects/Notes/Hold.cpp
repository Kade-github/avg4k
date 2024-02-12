#include "Hold.h"
#include "../../Helpers/TimeToScreen.h"
#include "../../A4kGame.h"

void Average4k::Objects::HoldNote::draw()
{
	if (missed)
		return;

	if (holding)
		hasHeld = true;

	if (lastHold != holding)
	{
		if (lastHold && !holding) // if you were holding last frame, and you stopped holding this frame
			endHold = currentBeat;
		lastHold = holding;
	}

	// center
	transform.x -= transform.w / 2;
	transform.y -= transform.h / 2;

	std::vector<AvgEngine::Render::Vertex> receptor = AvgEngine::Render::DisplayHelper::RectToVertex({ transform.x, transform.y + (transform.h / 2), transform.w, transform.h}, src);

	float endPosition = Average4k::Helpers::TimeToScreen::YOffset(cmod, endTime - noteTime);

	setPosition(); // Objects::BaseNote

	// welcome to the jungle, we got fun and games, we got everything you want, honey we know the names, we are the people that can find, whatever you may need, 
	// if you got the money honey, we got your disease. 
	// - github copilot

	// (aka holds)


	int amount = 1 + (endPosition / (transform.h));

	if (useXmod)
		amount = 1 + ((endBeat - data.beat) * (transform.h * xmod)) / (transform.h);

	if (amount < 0)
		amount = 0;

	using namespace AvgEngine;

	float w = src.w;
	float h = src.h;

	Render::Rect bodySrc = { sheet_col * w, sheet_row * h, w,h };
	Render::Rect endSrc = { sheet_end_col * w, sheet_end_row * h, w,h };

	if (!downscroll) // flip
		endSrc = { sheet_end_col * w, (sheet_end_row + 1) * h, w,-h };

	std::vector<std::vector<Render::Vertex>> vertices;

	float progress = -1;

	if (holding)
	{
		progress = (currentBeat - data.beat) / (endBeat - noteBeat);
	}
	else if (endHold >= 0)
	{
		progress = (endHold - data.beat) / (endBeat - noteBeat);
	}

	if (progress > 1)
		progress = 1;

	for (int i = 0; i < amount; i++)
	{
		Render::Rect r = { transform.x, transform.y + (transform.h / 2) + (transform.h * i), transform.w, transform.h };

		if (downscroll)
			r.y = transform.y - (transform.h * i);

		float p2 = (float)i / (float)amount;
		float p3 = (float)(i + 1) / (float)amount;

		std::vector<Render::Vertex> v = {};

		if (i + 1 != amount)
			v = Render::DisplayHelper::RectToVertex(r, bodySrc);
		else
			v = Render::DisplayHelper::RectToVertex(r, endSrc);

		// where da quad at, here it is:
		// [0] tl
		// [1] bl
		// [2] tr
		// [3] tr
		// [4] bl
		// [5] br

		if (!downscroll)
		{
			if (i == 0)
			{
				v[0].y = transform.y + (transform.h / 2); // top left
				v[0].x = transform.x;
				v[2].y = transform.y + (transform.h / 2); // top right
				v[2].x = transform.x + transform.w;
				v[3].y = transform.y + (transform.h / 2); // top right
				v[3].x = transform.x + transform.w;
			}
			else
			{
				std::vector<Render::Vertex>& last = vertices.back();
				v[0].y = last[1].y; // set the top left to the bottom left y
				v[0].x = last[1].x; // set the top left to the bottom left x
				v[2].y = last[5].y; // set the top right to the bottom right y
				v[2].x = last[5].x; // set the top right to the bottom right x
				v[3].y = last[5].y; // set the top right to the bottom right y
				v[3].x = last[5].x; // set the top right to the bottom right x
			}
		}
		else
		{
			if (i == 0)
			{
				v[1].y = transform.y - (transform.h / 2); // bottom left
				v[1].x = transform.x;
				v[4].y = transform.y - (transform.h / 2); // bottom left
				v[4].x = transform.x;
				v[5].y = transform.y - (transform.h / 2); // bottom right
				v[5].x = transform.x + transform.w;
			}
			else
			{
				std::vector<Render::Vertex>& last = vertices.back();
				v[1].y = last[0].y; // set the bottom left to the top left y
				v[1].x = last[0].x; // set the bottom left to the top left x
				v[4].y = last[0].y; // set the bottom left to the top left y
				v[4].x = last[0].x; // set the bottom left to the top left x
				v[5].y = last[2].y; // set the bottom right to the top right y
				v[5].x = last[2].x; // set the bottom right to the top right x
			}
		}

		// hold cliping

		if (p2 <= progress)
		{
			if (!downscroll) // top
			{
				v[0].y = receptor[0].y;
				v[2].y = receptor[2].y;
				v[3].y = receptor[3].y;
			}
			else // bottom
			{
				v[1].y = receptor[1].y;
				v[4].y = receptor[4].y;
				v[5].y = receptor[5].y;
			}
		}

		if (p3 <= progress)
		{
			if (!downscroll) // bottom
			{
				v[1].y = v[0].y;
				v[4].y = v[0].y;
				v[5].y = v[2].y;
			}
			else // top
			{
				v[0].y = v[1].y;
				v[2].y = v[5].y;
				v[3].y = v[5].y;
			}
		}

		vertices.push_back(v);
	}

	for (auto& v : vertices) // draw the hold
	{
		Base::drawCall c = Base::Camera::FormatDrawCall(zIndex, texture, shader, v, iTransform);
		camera->addDrawCall(c);
	}

	transform.x += transform.w / 2;
	transform.y += transform.h / 2;

	if (!hasHeld)
		Base::Sprite::draw();

	if (progress > 0 && progress < 1)
		A4kGame::gameInstance->DrawOutlinedDebugText(receptor[0].x, receptor[0].y - 32, std::to_string(progress), 32);


}
