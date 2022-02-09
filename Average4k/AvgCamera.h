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

	AvgCamera(int _x, int _y, int _w, int _h)
	{
		handleDraw = true;
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		w = _w;
		h = _h;
		x = _x;
		y = _y;
		tint.r = -1;

		ctb = new Texture(NULL, _w, _h);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctb->id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float shakeTime = 0;
	float shakeInten;
	void shake(float time, float inten)
	{
		shakeTime = time;
		shakeInten = inten;
	}

	void resChange(int w, int h)
	{
		glDeleteTextures(1, &ctb->id);
		glDeleteFramebuffers(1, &fb);

		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		w = w;
		h = h;
		ctb = new Texture(NULL, w, h);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctb->id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void draw();

};