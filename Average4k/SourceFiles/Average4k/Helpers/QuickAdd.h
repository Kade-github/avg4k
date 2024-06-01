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

		static AvgEngine::Base::Sprite* AddSprite(std::shared_ptr<AvgEngine::Base::GameObject> addTo, std::string file, int x, int y)
		{
			std::shared_ptr<AvgEngine::Base::Sprite> s = std::make_shared<AvgEngine::Base::Sprite>(x, y, file);

			addTo->addObject(s);

			return s;
		}

		static AvgEngine::Base::Text* AddText(std::shared_ptr<AvgEngine::Base::GameObject> addTo, std::string text, int size, int x, int y, std::string font = "KadeArial.fnt")
		{
			std::shared_ptr <AvgEngine::Base::Text> t = std::make_shared<AvgEngine::Base::Text>(x, y, "Assets/Fonts/", font, text, size);

			addTo->addObject(t);

			return t;
		}
	};
}

#endif