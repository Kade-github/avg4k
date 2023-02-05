#include "Camera.h"


Camera::Camera(int ww, int hh)
{

	w = ww;
	h = hh;

	std::srand(std::time(nullptr));
}

void Camera::update(Events::updateEvent event)
{
	if (shaking)
	{
		int mi = min * shakeIntensity;
		int ma = max * shakeIntensity;
		x = mi + rand() % ((ma + mi) - mi);
		y = mi + rand() % ((ma + mi) - min);


		if (SDL_GetTicks() > shakeTimer)
		{
			shaking = false;
			shakeIntensity = 0;
			x = 0;
			y = 0;
		}
	}
}

void Camera::shakeEffect(float intensity, int ms)
{
	shakeTimer = SDL_GetTicks() + ms;
	shakeIntensity = intensity;
	shaking = true;
}
