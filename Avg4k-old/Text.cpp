#include "Text.h"


void Text::setText(std::string temp)
{
	if (temp.size() == 0 || temp == " " || Arial == NULL)
		return;

	text = temp;

	if (fontMap.size() > 0)
		return;

	if (!alreadySet)
		for (int i = 0; i < text.size(); i++)
		{
			if ((char)text[i] > 128 || (char)text[i] < 0)
			{
				if (curFont != "arial")
				{
					alreadySet = true;
					setFont("arial");
				}
				return;
			}
			else if (ogFont != "arial" && curFont != ogFont)
			{
				setFont(ogFont);
				return;
			}
		}

	const char* c = text.c_str();
	SDL_Surface* borderMe = nullptr;
	
	if (border)
	{
		TTF_SetFontOutline(Arial, borderSize);

		borderMe =
			TTF_RenderUTF8_Blended(Arial, c, { 255, 255, 255 });
	}
	TTF_SetFontOutline(Arial, 0);
	SDL_Surface* surfaceMessage =
		TTF_RenderUTF8_Blended(Arial, c, { 255, 255,255 });
	
	if (surfaceMessage != nullptr)
	{
		if (message)
			delete message;

		if (outline)
			delete outline;

		if (borderMe)
		{
			outline = Texture::createFromSurface(borderMe, true);
			borderW = outline->width;
			borderH = outline->height;
		}

		message = Texture::createFromSurface(surfaceMessage, true);
		w = message->width;
		h = message->height;
		surfW = message->width;
		surfH = message->height;
	}
	alreadySet = false;
}

void Text::setCharacterSpacing(float spacing)
{
	TTF_SetFontCharacterSpacing(Arial, spacing);
	setText(text);
}

void Text::draw()
{
	if (fontMap.size() > 0)
	{
		// draw font map
		Rect dstRect;
		Rect srcRect;

		int totalW = 0;
		w = 0;
		h = 0;

		for (char c : text)
		{
			Texture* t = fontMap[c];

			if (t == NULL)
				continue;
			totalW += (t->width * fontMapSize) + fontMapSpacing;
			if (t->height * fontMapSize > h)
				h = t->height * fontMapSize;
		}
		h *= scale;
		w = totalW * scale;

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		dstRect.a = alpha;

		int i = 0;

		float startX = x - (totalW / 2);

		for (char c : text)
		{

			dstRect.y = y;
			Texture* t = fontMap[c];

			if (t == NULL)
			{
				i++;
				continue;
			}

			dstRect.x = startX;
			float mpx = ((t->width * fontMapSize) * (1 - scale)) / 2;
			float mpy = ((t->height * fontMapSize) * (1 - scale)) / 2;


			dstRect.x += mpx;
			dstRect.y += mpy;

			dstRect.w = (t->width * fontMapSize) * scale;
			dstRect.h = (t->height * fontMapSize) * scale;

			startX += (t->width * fontMapSize) + fontMapSpacing;

			Rendering::PushQuad(&dstRect, &srcRect, t, GL::genShader, 0);
			i++;
		}
		return;
	}
	if (text.size() == 0 || text == " " || !message || !drawCall)
		return;

	Rect dstRect;
	Rect srcRect;

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 1;
	srcRect.h = 1;

	float mpx = (w * (1 - scale)) / 2;
	float mpy = (h * (1 - scale)) / 2;


	dstRect.x = x + mpx;
	dstRect.y = y + mpy;
	dstRect.w = w * scale;
	dstRect.h = h * scale;
	dstRect.a = alpha;


	if (border && outline)
	{
		dstRect.x -= borderSize;
		dstRect.y -= borderSize;
		dstRect.w = borderW * scale;
		dstRect.h = borderH * scale;
		dstRect.a = borderAlpha;
		dstRect.r = borderColor.r;
		dstRect.g = borderColor.g;
		dstRect.b = borderColor.b;
		Rendering::PushQuad(&dstRect, &srcRect, outline, GL::genShader, angle);
		dstRect.x += borderSize;
		dstRect.y += borderSize;
		dstRect.w = w * scale;
		dstRect.h = h * scale;
		dstRect.a = alpha;
	}
	dstRect.r = color.r;
	dstRect.g = color.g;
	dstRect.b = color.b;

	Rendering::PushQuad(&dstRect, &srcRect, message, GL::genShader, angle);
}

void Text::setFont(std::string name)
{
	curFont = name;
	Arial = Font::getFontByName(name, size);
	setText(text);
}

void Text::forceDraw()
{
}

void Text::die()
{
	if (!this)
		return;
	isDead = true;
	if (message)
		if (message->width > 0 && message->height > 0)
			delete message;
}

void Text::centerX()
{
	if (surfW == 0)
	{
		// lol
	}
	else
		setX((Game::gameWidth / 2) - (surfW / 2));
}

void Text::centerY()
{
	if (surfH == 0)
	{
		// lol
	}
	else
		setX((Game::gameWidth / 2) - (surfH / 2));
}
