#include "Text.h"

void Text::setText(std::string temp)
{
	if (temp.size() == 0 || temp == " " || Arial == NULL)
		return;

	text = temp;

	const char* c = text.c_str();
	SDL_Surface* borderMe = nullptr;
	
	if (border)
	{
		TTF_SetFontOutline(Arial, borderSize);

		borderMe =
			TTF_RenderText_Blended(Arial, c, { 255, 255, 255 });
	}
	TTF_SetFontOutline(Arial, 0);
	SDL_Surface* surfaceMessage =
		TTF_RenderText_Blended(Arial, c, { 255, 255,255 });
	
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
}

void Text::setCharacterSpacing(float spacing)
{
	TTF_SetFontCharacterSpacing(Arial, spacing);
	setText(text);
}

void Text::draw()
{
	if (text.size() == 0 || text == " " || !message)
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
		std::cout << "bruh??? surfW is zero (divide by zero)" << std::endl;
	else
		setX((Game::gameWidth / 2) - (surfW / 2));
}

void Text::centerY()
{
	if (surfH == 0)
		std::cout << "bruh??? surfH is zero (divide by zero)" << std::endl;
	else
		setX((Game::gameWidth / 2) - (surfH / 2));
}
