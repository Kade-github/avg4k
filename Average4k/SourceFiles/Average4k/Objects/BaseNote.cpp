/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "BaseNote.h"
#include "../Helpers/TimeToScreen.h"

void Average4k::Objects::BaseNote::setPosition()
{
	if (!downscroll)
	{
		if (cmod > 0)
			transform.y += Average4k::Helpers::TimeToScreen::YOffset(cmod, noteTime - currentTime);
		else
			transform.y += Average4k::Helpers::TimeToScreen::YOffsetXMod(xmod, data.beat - currentBeat);
	}
	else
	{
		if (cmod > 0)
			transform.y -= Average4k::Helpers::TimeToScreen::YOffset(cmod, noteTime - currentTime);
		else
			transform.y -= Average4k::Helpers::TimeToScreen::YOffsetXMod(xmod, data.beat - currentBeat);
	}
}

void Average4k::Objects::BaseNote::draw()
{
	setPosition();
	AvgEngine::Base::Sprite::draw();

	if (overlays.size() == 2)
	{
		AvgEngine::Render::Rect srcSave = src;
		AvgEngine::Render::Rect transformSave = transform;

		float w = src.w;
		float h = src.h;

		transform.angle = overlayAngle;
		transform.a = overlayOpacity;
		src = { overlays[0] * w, overlays[1] * h, w, h };
		AvgEngine::Base::Sprite::draw();
		src = srcSave;
		transform = transformSave;
	}
}