#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "steam/steam_api.h"

class Events
{
public:
	struct updateEvent {
		SDL_Renderer* renderer;
		SDL_Window* window;
	};

};