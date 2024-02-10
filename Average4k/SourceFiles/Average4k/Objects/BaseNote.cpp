/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "BaseNote.h"
#include "../Helpers/TimeToScreen.h"

void Average4k::Objects::BaseNote::draw()
{
	if (!downscroll)
	{
		if (cmod > 0)
			transform.y += Average4k::Helpers::TimeToScreen::YOffset(cmod, data.beat - currentBeat);
		else
			transform.y += Average4k::Helpers::TimeToScreen::YOffsetXMod(xmod, data.beat - currentBeat);
	}
	else
	{
		if (cmod > 0)
			transform.y -= Average4k::Helpers::TimeToScreen::YOffset(cmod, data.beat - currentBeat);
		else
			transform.y -= Average4k::Helpers::TimeToScreen::YOffsetXMod(xmod, data.beat - currentBeat);
	}

	AvgEngine::Base::Sprite::draw();
}