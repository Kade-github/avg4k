#include "Playfield.h"
#include "Average4K.h"

Average4k::Objects::Gameplay::Playfield::Playfield(int _x, int _y) : AvgEngine::Base::GameObject(_x, _y)
{
	arrowSpritesheet = Average4K::skin->GetArrowTexture("arrows");
	receptorSpritesheet = Average4K::skin->GetArrowTexture("receptors");
	float noteSize = std::stof(Average4K::settings->Get("Note Size").value);
	for (int i = 0; i < 4; i++)
	{
		Receptor* r = new Receptor((i * (64 * noteSize)), 0, receptorSpritesheet, arrowSpritesheet);
		r->SetFrameSize(64,64);
		r->transform.w = 64 * noteSize;
		r->transform.h = 64 * noteSize;
		receptors.push_back(r);
		addObject(receptors[i]);
	}
	receptors[0]->transform.angle = 90;
	receptors[1]->transform.angle = 0;
	receptors[2]->transform.angle = 180;
	receptors[3]->transform.angle = -90;
}