#include "Text.h"

void Text::setText(std::string temp)
{
	this->text = temp;

	SDL_Surface* screen = SDL_GetWindowSurface(Game::window);

	if (message != nullptr)
		SDL_DestroyTexture(message);

	SDL_Surface* surfaceMessage =
		TTF_RenderText_Blended_Wrapped(GetFont(), text.c_str(), { color.r, color.g,color.b }, screen->w);
	surfW = surfaceMessage->w;
	surfH = surfaceMessage->h;

	message = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);
}

void Text::update(Events::updateEvent ev)
{
	SDL_QueryTexture(message, NULL, NULL, &rW, &rH);

	message_Rect.x = x;
	message_Rect.y = y;
	message_Rect.w = rW;
	message_Rect.h = rH;

	SDL_RenderCopyF(ev.renderer, message, NULL, &message_Rect);
}
