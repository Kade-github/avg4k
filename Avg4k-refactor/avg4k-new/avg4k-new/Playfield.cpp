#include "Playfield.h"
#include "Average4K.h"
#include "StringTools.h"

Average4k::Objects::Gameplay::Playfield::Playfield(int _x, int _y, Average4k::Chart::Difficulty* d) : AvgEngine::Base::GameObject(_x, _y)
{
	diff = d;
	arrowSpritesheet = Average4K::skin->GetArrowTexture("arrows", true);
	receptorSpritesheet = Average4K::skin->GetArrowTexture("receptors", true);
	holdSpritesheet = Average4K::skin->GetArrowTexture("holds", true);
	expSpritesheet = Average4K::skin->GetArrowTexture("explosion", true);
	arrowSpritesheet->dontDelete = true;
	receptorSpritesheet->dontDelete = true;
	holdSpritesheet->dontDelete = true;
	expSpritesheet->dontDelete = true;

	float noteSize = std::stof(Average4K::settings->Get("Note Size").value);
	for (int i = 0; i < 4; i++)
	{
		Receptor* r = new Receptor((i * (64 * noteSize)), 0, receptorSpritesheet, arrowSpritesheet, expSpritesheet);
		r->SetFrameSize(64,64);
		r->holdSpritesheet = holdSpritesheet;
		r->tag = "receptor-" + std::to_string(i);
		r->transform.w = 64 * noteSize;
		r->transform.h = 64 * noteSize;
		r->center = true;
		r->lane = i;
		r->noteSize = noteSize;
		r->explosion->transform.w = (64 * noteSize) * 2;
		r->explosion->transform.h = (64 * noteSize) * 2;
		for (Chart::Note& n : diff->Notes)
		{
			if (n.Lane == i)
				r->notes.push_back(n);
		}
		receptors.push_back(r);
		addObject(receptors[i]);
	}
	if (Average4K::skin->rotate)
	{
		receptors[0]->transform.angle = 90;
		receptors[1]->transform.angle = 0;
		receptors[2]->transform.angle = 180;
		receptors[3]->transform.angle = -90;
	}

	keybinds = { 
		AvgEngine::Utils::StringTools::convertKeyNameToKeyCode(Average4K::settings->Get("Left Keybind").value),
		AvgEngine::Utils::StringTools::convertKeyNameToKeyCode(Average4K::settings->Get("Down Keybind").value),
		AvgEngine::Utils::StringTools::convertKeyNameToKeyCode(Average4K::settings->Get("Up Keybind").value),
		AvgEngine::Utils::StringTools::convertKeyNameToKeyCode(Average4K::settings->Get("Right Keybind").value) 
	};
}

void Average4k::Objects::Gameplay::Playfield::draw()
{
	Average4K* k = static_cast<Average4K*>(Average4K::Instance);

	for (Receptor* r : receptors)
	{
		r->time = time;
		r->beat = beat;
	}

	AvgEngine::Base::GameObject::draw();
}
