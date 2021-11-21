#include "Text.h"
#include "Game.h"

void Text::setText(std::string temp)
{

	std::string* _text = new std::string(temp);

	delete text;

	this->text = _text;

	SDL_Surface* screen = SDL_GetWindowSurface(Game::window);

	if (surfaceMessage != nullptr)
	{
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(message);
	}

	static TTF_Font* Sans = nullptr;

	if (!Sans)
		Sans = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 24); // we support linux :)

	surfaceMessage =
		TTF_RenderText_Blended_Wrapped(Sans, text->c_str(), { color.r, color.g,color.b }, screen->w);
	message = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessage);
}

void Text::update(Events::updateEvent ev)
{
	SDL_QueryTexture(message, NULL, NULL, &rW, &rH);

	SDL_Surface* screen = SDL_GetWindowSurface(Game::window);

	if (surfaceMessage != nullptr)
	{
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(message);
	}

	static TTF_Font* Sans = nullptr;

	if (!Sans)
		Sans = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 24); // we support linux :)

	surfaceMessage =
		TTF_RenderText_Blended_Wrapped(Sans, text->c_str(), { color.r, color.g,color.b }, screen->w);
	message = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessage);

	message_Rect.x = x;
	message_Rect.y = y;
	message_Rect.w = rW;
	message_Rect.h = rH;

	SDL_RenderCopy(ev.renderer, message, NULL, &message_Rect);
}
