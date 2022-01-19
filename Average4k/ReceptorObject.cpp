#include "ReceptorObject.h"
#include "Gameplay.h"

void ReceptorObject::draw() {


	SDL_FRect rect;

	float mpx = (w * (1 - scale)) / 2;
	float mpy = (h * (1 - scale)) / 2;

	float scaledWidth = w * scale;
	float scaledHeight = h * scale;

	rect.w = scaledWidth;
	rect.h = scaledHeight;
	rect.x = x + mpx;
	rect.y = y + mpy;

	Rect dstRect;
	Rect srcRect;

	dstRect.x = rect.x;
	dstRect.y = rect.y;
	dstRect.w = rect.w;
	dstRect.h = rect.h;
	dstRect.r = 255;
	dstRect.g = 255;
	dstRect.b = 255;
	dstRect.a = alpha;

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 1;
	srcRect.h = 1;


	Texture* receptor = Gameplay::noteskin->receptor;

	if (Gameplay::noteskin->rotate)
	{
		switch (type)
		{
		case 0:
			if (lightUpTimer <= 0)
				Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->receptor, GL::genShader, 270);
			else
				Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->light, GL::genShader, 270);

			break;
		case 1:
			if (lightUpTimer <= 0)
				Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->receptor, GL::genShader);
			else
				Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->light, GL::genShader);
			break;
		case 2:
			srcRect.h = -1;
			if (lightUpTimer <= 0)
				Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->receptor, GL::genShader);
			else
				Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->light, GL::genShader);
			srcRect.h = 1;
			break;
		case 3:
			if (lightUpTimer <= 0)
				Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->receptor, GL::genShader, 90);
			else
				Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->light, GL::genShader, 90);
			break;
		}
	}
	else
	{
		if (lightUpTimer <= 0)
			Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->receptor, GL::genShader);
		else
			Rendering::PushQuad(&dstRect, &srcRect, Gameplay::noteskin->light, GL::genShader);
	}

	if (lightUpTimer > 0)
	{
		lightUpTimer = lightUpTimer - Game::deltaTime;
	}
	Rendering::drawBatch();
}