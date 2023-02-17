#pragma once
#include "includes.h"
#include "Sprite.h"
#include "Skin.h"
#include "Display.h"
class Container : public AvgEngine::Base::Sprite
{
	Average4k::Skin* s;
public:
	AvgEngine::Render::Rect cTransform;
	Container(Average4k::Skin* _s, float _x, float _y, std::string container) : Sprite(_x, _y, _s->GetTexture("Menu/" + container))
	{
		s = _s;
		transformRatio = true;
	}

	void Added() override
	{
		cTransform = transform;
	}

	void draw() override
	{
		transform = cTransform;
		transform.w *= s->upscale;
		transform.h *= s->upscale;
	}
};
