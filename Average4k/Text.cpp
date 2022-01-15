#include "Text.h"

void Text::setText(std::string temp)
{
	this->text = temp;
	if (temp == "" || Arial == NULL)
		return;
	//std::cout << TTF_FontFaceFamilyName(Arial) << std::endl;

	//SDL_Surface* screen = SDL_GetWindowSurface(Game::window);

	if (message != nullptr)
		SDL_DestroyTexture(message);
	if (outline != nullptr)
		SDL_DestroyTexture(outline);

	SDL_Surface* surfaceMessage =
		TTF_RenderUTF8_Blended_Wrapped(Arial, text.c_str(), { color.r, color.g,color.b }, Game::gameWidth);
	SDL_Surface* outlineMsg =
		TTF_RenderUTF8_Blended_Wrapped(Arial, text.c_str(), { 0,0,0 }, Game::gameWidth);


	if (surfaceMessage != nullptr && outlineMsg != nullptr)
	{

		surfW = surfaceMessage->w;
		surfH = surfaceMessage->h;

		outline = SDL_CreateTextureFromSurface(Game::renderer, outlineMsg);
		message = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessage);

		SDL_QueryTexture(message, NULL, NULL, &rW, &rH);

		SDL_FreeSurface(outlineMsg);
		SDL_FreeSurface(surfaceMessage);
	}
}
void Text::draw()
{
	SDL_FRect message_Rect;
	message_Rect.x = x;
	message_Rect.y = y;
	message_Rect.w = rW * scale;
	message_Rect.h = rH * scale;

	SDL_FRect rect;
	rect.x = message_Rect.x - (size / 8);
	rect.y = message_Rect.y;
	rect.w = rW * scale;
	rect.h = rH * scale;
	
	SDL_SetTextureAlphaMod(message, alpha);
	if (border)
	{
		SDL_SetTextureAlphaMod(outline, alpha);
		SDL_RenderCopyF(Game::renderer, outline, NULL, &rect);
	}

	SDL_RenderCopyF(Game::renderer, message, NULL, &message_Rect);
}

void Text::setFont(std::string name)
{
	Arial = Font::getFontByName(name, size);
	setText(text);
}

void Text::forceDraw()
{
	SDL_FRect message_Rect;
	message_Rect.x = x;
	message_Rect.y = y;
	message_Rect.w = rW * scale;
	message_Rect.h = rH * scale;

	SDL_RenderCopyF(Game::renderer, message, NULL, &message_Rect);
}

void Text::die()
{
	if (!this)
		return;
	isDead = true;
	SDL_DestroyTexture(outline);
	SDL_DestroyTexture(message);
	if (isCreated)
		Game::removeGlobalObject(this);
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
