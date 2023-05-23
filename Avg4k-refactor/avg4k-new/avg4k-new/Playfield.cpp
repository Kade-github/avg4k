#include "Playfield.h"
#include "Average4K.h"

Average4k::Objects::Gameplay::Playfield::Playfield(int _x, int _y) : AvgEngine::Base::GameObject(_x, _y)
{
	arrowSpritesheet = Average4K::skin->GetArrowTexture("arrows");
	receptorSpritesheet = Average4K::skin->GetArrowTexture("receptors");
	float noteSize = std::stof(Average4K::settings->Get("Note Size").value);
	for (int i = 0; i < 4; i++)
	{
		receptors.push_back(new Receptor((i * (64 * noteSize)), 0, receptorSpritesheet, arrowSpritesheet));
		addObject(receptors[i]);
	}
}