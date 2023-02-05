#pragma once
#include "Object.h"

class Camera : public Object
{
public:
	int w = 0;
	int h = 0;

	float angle = 0;

	bool shaking = false;
	float shakeIntensity = 0;
	int shakeTimer = 0;

	int min = -8;
	int max = 8;

	Camera(int ww, int hh);
	~Camera() = default;

	SDL_Texture* cameraTexture;

	void update(Events::updateEvent event) override;

	void shakeEffect(float intensity, int ms);

};

