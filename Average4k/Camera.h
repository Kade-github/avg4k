#pragma once
#include "Object.h"

class Camera : public Object
{
public:
	Camera();
	~Camera() = default;

	SDL_Texture* cameraTexture;

	void update(Events::updateEvent event) override;

	void shakeEffect(float intensity, int ms);

	float angle = 0;

	int w = 792;
	int h = 600;
};

