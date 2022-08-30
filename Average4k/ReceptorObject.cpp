#include "ReceptorObject.h"
#include "Gameplay.h"
#include "ArrowEffects.h"


ReceptorObject::ReceptorObject(int _x, int _y, int _type)
{
	MUTATE_START
		x = _x;
	y = _y;
	// this does the same thing but im lazy and too lazy to check
	setX(_x);
	setY(_y);
	w = (64 * Game::save->GetDouble("Note Size"));
	h = (64 * Game::save->GetDouble("Note Size"));

	if (ModManager::doMods)
	{
		modX = x;
		modY = y;
	}

	type = _type;
	scale = 1;
	MUTATE_END
}

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
	if (lightUpTimer > 0)
		alpha = std::lerp(0, 1, lightUpTimer / 150);
	dstRect.a = alpha;

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 1;
	srcRect.h = 1;

	float drawAngle = 0;

	if (ModManager::doMods)
	{
		ArrowEffects::Arrow a = ArrowEffects::finishEffects(x, y, type, Gameplay::instance->positionInSong);
		dstRect.x = a.x + mpx;
		dstRect.y = a.y + mpy;
		drawAngle = a.rot;
	}

	modX = dstRect.x;
	modY = dstRect.y;

	Shader* sh = customShader;

	if (!sh)
		sh = GL::genShader;

	Texture* receptor = Game::noteskin->receptor;

	if (Game::noteskin->rotate)
	{
		switch (type)
		{
		case 0:
			dstRect.a = defAlpha;
			Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, 90 + drawAngle);
			dstRect.a = alpha;
			if (lightUpTimer > 0)
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, 90 + drawAngle);
			

			break;
		case 1:
			dstRect.a = defAlpha;
			Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, drawAngle);
			dstRect.a = alpha;
			if (lightUpTimer > 0)
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, drawAngle);
			break;
		case 2:
			srcRect.h = -1;
			dstRect.a = defAlpha;
			Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, drawAngle);
			dstRect.a = alpha;
			if (lightUpTimer > 0)
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, drawAngle);
			srcRect.h = 1;
			break;
		case 3:
			dstRect.a = defAlpha;
			Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, -90 + drawAngle);
			dstRect.a = alpha;
			if (lightUpTimer > 0)
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, -90 + drawAngle);
			break;
		}
	}
	else
	{
		dstRect.a = defAlpha;
		Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, drawAngle);
		dstRect.a = alpha;
		if (lightUpTimer > 0)
			Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, drawAngle);
	}

	if (lightUpTimer > 0)
	{
		lightUpTimer = lightUpTimer - Game::deltaTime;
	}
	Rendering::drawBatch();
}