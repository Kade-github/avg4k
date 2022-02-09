#pragma once
#include "includes.h"
#include "GL.h"
#include "Object.h"

class AvgGroup : public Object
{
public:
	GLuint fb; // Frame buffer
	Texture* ctb; // Color texture buffer

	Rect clipRect;

	bool center = true;

	AvgGroup(int _x, int _y, int _w, int _h) : Object(x, y)
	{
		handleDraw = true;
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		w = _w;
		h = _h;
		x = _x;
		y = _y;

		ctb = new Texture(NULL, _w, _h);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctb->id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		else
			std::cout << "created frame buffer " << fb << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	~AvgGroup()
	{
		if (ctb)
			delete ctb;
	}

	virtual void draw();
};