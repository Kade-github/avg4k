#pragma once
#include "AvgGroup.h"
#include "includes.h"

struct AvgColorTint {
	int r, g, b;
};

class AvgCamera : public Object
{
public:

	GLuint fb; // Frame buffer
	Texture* ctb; // Color texture buffer
	AvgColorTint tint;

	Rect clipRect;

	float initalW, initalH;

	AvgCamera(int _x, int _y, int _w, int _h)
	{
		MUTATE_START
		id = 690690690;
		handleDraw = true;
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		w = _w;
		h = _h;
		x = _x;
		y = _y;
		tint.r = -1;

		initalW = _w;
		initalH = _h;

		ctb = new Texture(NULL, _w, _h);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctb->id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		MUTATE_END
	}

	float shakeTime = 0;
	float shakeInten;
	void shake(float time, float inten)
	{
		shakeTime = time;
		shakeInten = inten;
	}

	void resize(float _w, float _h)
	{
		w = _w;
		h = _h;

		float aspect = w / h;

	}

	virtual void draw();

};