#include "Hold.h"
#include "../../Helpers/TimeToScreen.h"

void Average4k::Objects::HoldNote::draw()
{
	float endPosition = Average4k::Helpers::TimeToScreen::YOffset(cmod, endBeat - data.beat);

	setPosition(); // Objects::BaseNote

	// welcome to the jungle, we got fun and games, we got everything you want, honey we know the names, we are the people that can find, whatever you may need, 
	// if you got the money honey, we got your disease. 
	// - github copilot

	// (aka holds)

	int amount = 1 + static_cast<int>((endPosition - transform.h) / transform.h);

	using namespace AvgEngine;

	float w = src.w;
	float h = src.h;

	Render::Rect bodySrc = { sheet_col * w, sheet_row * h, w,h };
	Render::Rect endSrc = { sheet_end_col * w, sheet_end_row * h, w,h };

	if (!downscroll) // flip
		endSrc = { sheet_end_col * w, (sheet_end_row + 1) * h, w,-h };

	std::vector<std::vector<Render::Vertex>> vertices;

	for (int i = 0; i < amount; i++)
	{
		Render::Rect r = { transform.x, transform.y + (transform.h / 2) + (transform.h * i), transform.w, transform.h };

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

		vertices.push_back(v);
	}

	for (auto& v : vertices) // draw the hold
	{
		Base::drawCall c = Base::Camera::FormatDrawCall(zIndex, texture, shader, v, iTransform);
		camera->addDrawCall(c);
	}

	Base::Sprite::draw();

}
