#include "Hold.h"
#include "../../Helpers/TimeToScreen.h"
#include "../../A4kGame.h"
#include "../../Screens/Menu/Gameplay.h"

float Average4k::Objects::HoldNote::calculateHoldHeight()
{
	float height = 0;

	if (!downscroll)
		height = Average4k::Helpers::TimeToScreen::YOffset(cmod, endTime - noteTime);
	else
		height = Average4k::Helpers::TimeToScreen::YOffset(cmod, noteTime - endTime);

	int amount = (height / (transform.h));

	if (useXmod)
		amount = ((endBeat - data.beat) * (transform.h * xmod)) / (transform.h);

	float noteScaleH = Average4k::Api::Functions::FGame::GetHeightScale();

	// (pixel height / (noteHeight)) * scale

	return amount * noteScaleH;
}

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

	using namespace AvgEngine;

	float w = src.w;
	float h = src.h;

	Render::Rect bodySrc = { sheet_col * w, sheet_row * h, w,h };
	Render::Rect endSrc = { sheet_end_col * w, sheet_end_row * h, w,h };

	std::vector<AvgEngine::Render::Vertex> receptor = AvgEngine::Render::DisplayHelper::RectToVertex({ transform.x, transform.y + (transform.h / 2), transform.w, transform.h }, bodySrc);

	float endPosition = Average4k::Helpers::TimeToScreen::YOffset(cmod, endTime - noteTime);

	float rEnd = transform.y + endPosition;

	setPosition(); // Objects::BaseNote

	if ((transform.y - transform.h > Render::Display::height && !downscroll) ||
		(transform.y + transform.h < 0 && downscroll))
		return;

	// welcome to the jungle, we got fun and games, we got everything you want, honey we know the names, we are the people that can find, whatever you may need, 
	// if you got the money honey, we got your disease. 
	// - github copilot

	// (aka holds)


	if (!downscroll) // flip
		endSrc = { sheet_end_col * w, (sheet_end_row + 1) * h, w,-h };

	std::vector<std::vector<Render::Vertex>> vertices;
	std::shared_ptr<Average4k::Screens::Menu::Gameplay> gameplay = std::dynamic_pointer_cast<Average4k::Screens::Menu::Gameplay>(Average4k::A4kGame::gameInstance->CurrentMenu);

	// draw hold

	float sHoldBeat = noteBeat;

	if (!holding && hasHeld)
		sHoldBeat = endHold;

	

	
	for (auto& v : vertices) // draw the hold
	{
		Base::drawCall c = Base::Camera::FormatDrawCall(zIndex, texture, shader, v, iTransform);

		camera->addDrawCall(c);
	}



	transform.x += transform.w / 2;
	transform.y += transform.h / 2;

	if (!hasHeld)
		Base::Sprite::draw();
}
