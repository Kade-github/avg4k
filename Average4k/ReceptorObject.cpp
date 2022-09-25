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
	w = (64 * ArrowEffects::noteSize);
	h = (64 * ArrowEffects::noteSize);

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


	bool sparrow = Game::noteskin->sparrowImg;

	SDL_FRect rect = {};




	Rect dstRect;
	Rect srcRect;

	float mpx = (w * (1 - scale)) / 2;
	float mpy = (h * (1 - scale)) / 2;

	rect.x = x + mpx;
	rect.y = y + mpy;

	float drawAngle = 0;

	if (ModManager::doMods)
	{
		ArrowEffects::Arrow a = ArrowEffects::finishEffects(x, y, type, positionInSong);
		defAlpha = a.opac;
		drawAngle = a.rot;

		float size = ArrowEffects::noteSize * (0.5 / a.mini);

		x = ((Game::gameWidth / 2) - ((64 * size + 12) * 2)) + ((64 * size + 12) * type);

		w = 64 * size;
		h = 64 * size;
		mpx = (w * (1 - scale)) / 2;
		mpy = (h * (1 - scale)) / 2;

		dstRect.x = a.x + mpx;
		dstRect.y = a.y + mpy;

		if (ArrowEffects::ShowSplines)
			ArrowEffects::drawLine(x, y, type, beat, currentChart);
	}
	else
	{
		dstRect.x = rect.x;
		dstRect.y = rect.y;
	}


	float scaledWidth = w * scale;
	float scaledHeight = h * scale;

	dstRect.w = scaledWidth;
	dstRect.h = scaledHeight;
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


	modX = dstRect.x;
	modY = dstRect.y;

	Shader* sh = customShader;

	if (!sh)
		sh = GL::genShader;


	if (!sparrow)
	{

		Texture* receptor = Game::noteskin->receptor;

		if (Game::noteskin->rotate)
		{
			switch (type)
			{
			case 0:
				dstRect.a = defAlpha;
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, 90 + drawAngle);
				dstRect.a = alpha * defAlpha;
				if (lightUpTimer > 0)
					Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, 90 + drawAngle);


				break;
			case 1:
				dstRect.a = defAlpha;
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, drawAngle);
				dstRect.a = alpha * defAlpha;
				if (lightUpTimer > 0)
					Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, drawAngle);
				break;
			case 2:
				srcRect.h = -1;
				dstRect.a = defAlpha;
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, drawAngle);
				dstRect.a = alpha * defAlpha;
				if (lightUpTimer > 0)
					Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, drawAngle);
				srcRect.h = 1;
				break;
			case 3:
				dstRect.a = defAlpha;
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, -90 + drawAngle);
				dstRect.a = alpha * defAlpha;
				if (lightUpTimer > 0)
					Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, -90 + drawAngle);
				break;
			}
		}
		else
		{
			dstRect.a = defAlpha;
			Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->receptor, sh, drawAngle);
			dstRect.a = alpha * defAlpha;
			if (lightUpTimer > 0)
				Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->light, sh, drawAngle);
		}
	}
	else
	{
		AvgSparrow* sparrow = Game::noteskin->sparrow;

		dstRect.a = defAlpha;

		animTime += Game::deltaTime;
		frame = (animTime * fps / 1000);

		std::string anim = "";

		switch (type)
		{
		case 0:
			anim = Game::noteskin->receptorLeft;
			break;
		case 1:
			anim = Game::noteskin->receptorDown;
			break;
		case 2:
			anim = Game::noteskin->receptorUp;
			break;
		case 3:
			anim = Game::noteskin->receptorRight;
			break;
		}

		if (lightUpTimer > 0)
		{
			switch (type)
			{
			case 0:
				if (!hit)
					anim = Game::noteskin->receptorLitLeft;
				else
					anim = Game::noteskin->receptorHitLeft;
				break;
			case 1:
				if (!hit)
					anim = Game::noteskin->receptorLitDown;
				else
					anim = Game::noteskin->receptorHitDown;
				break;
			case 2:
				if (!hit)
					anim = Game::noteskin->receptorLitUp;
				else
					anim = Game::noteskin->receptorHitUp;
				break;
			case 3:
				if (!hit)
					anim = Game::noteskin->receptorLitRight;
				else
					anim = Game::noteskin->receptorHitRight;
				break;
			}
		}

		if (lastFrame != anim)
		{
			animTime = 0;
			frame = 0;
		}

		lastFrame = anim;

		int size = sparrow->animations[anim].frames.size();
		if (frame > size - 1 && loop)
		{
			animTime = 0;
			frame = 0;
		}
		else if (frame > size - 1)
		{
			frame = size - 1;
			if (bot)
				lightUpTimer = 0;
		}


		float mpx = (w * ((1 - Game::noteskin->hitReceptorScale))) / 2;
		float mpy = (h * ((1 - Game::noteskin->hitReceptorScale))) / 2;

		if (hit)
		{
			dstRect.w = w * Game::noteskin->hitReceptorScale;
			dstRect.h = h * Game::noteskin->hitReceptorScale;
			dstRect.x += mpx - Game::noteskin->offsetXReceptorHit;
			dstRect.y += mpy - Game::noteskin->offsetYReceptorHit;
		}

		AvgFrame fr = sparrow->getRectFromFrame(anim, frame);
		srcRect = fr.srcRect;

		Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->sparrowImg, sh, drawAngle);
	}
	if (lightUpTimer > 0)
	{
		lightUpTimer = lightUpTimer - Game::deltaTime;
	}
	Rendering::drawBatch();
}