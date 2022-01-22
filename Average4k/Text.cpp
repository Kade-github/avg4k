#include "Text.h"

void Text::setText(std::string temp)
{
	if (temp.size() == 0 || temp == " " || Arial == NULL)
		return;

	text = temp;

	const char* c = text.c_str();

	SDL_Surface* surfaceMessage =
		TTF_RenderUTF8_Blended_Wrapped(Arial, c, { 255, 255,255 }, Game::gameWidth);
	if (surfaceMessage != nullptr)
	{
		if (message)
			delete message;

		message = Texture::createFromSurface(surfaceMessage, true);
		w = message->width;
		h = message->height;
		surfW = message->width;
		surfH = message->height;
	}
}
void Text::draw()
{
	if (text.size() == 0 || text == " ")
		return;
	Rect dstRect;
	Rect srcRect;

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 1;
	srcRect.h = 1;

	if (border)
	{
		dstRect.x = x - 1;
		dstRect.y = y;

		dstRect.w = w * scale;
		dstRect.h = h * scale;
		dstRect.r = 0;
		dstRect.g = 0;
		dstRect.b = 0;
		dstRect.a = alpha;
		Rendering::PushQuad(&dstRect, &srcRect, message, GL::genShader);
	}

	dstRect.x = x;
	dstRect.y = y;

	dstRect.w = w * scale;
	dstRect.h = h * scale;
	dstRect.r = color.r;
	dstRect.g = color.g;
	dstRect.b = color.b;
	dstRect.a = alpha;


	Rendering::PushQuad(&dstRect, &srcRect, message, GL::genShader);
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
	// this is a memory leak, but until I can figure out why the fuck deleting this causes a heap currupt.
	// we cannot
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
