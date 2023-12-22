/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_QUICKADD_H
#define AVG4K_QUICKADD_H

#pragma once

#include <AvgEngine/Base/Text.h>
#include <AvgEngine/Base/Sprite.h>

namespace Average4k::Helpers
{
	class QuickAdd
	{
	public:

		static AvgEngine::Base::Sprite* AddSprite(AvgEngine::Base::GameObject* addTo, std::string file, int x, int y)
		{
			AvgEngine::Base::Sprite* s = new AvgEngine::Base::Sprite(x, y, file);

			addTo->addObject(s);

			return s;
		}

		static AvgEngine::Base::Text* AddText(AvgEngine::Base::GameObject* addTo, std::string text, int size, int x, int y, std::string font = "KadeArial.fnt")
		{
			AvgEngine::Base::Text* t = new AvgEngine::Base::Text(x, y, "Assets/Fonts/", font, text, size);

			addTo->addObject(t);

			return t;
		}
	};
}

#endif