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
	bool renderOnce = false;
	bool flip = false;

	float newW, newH;
	float initalW, initalH;

	void bind(float _w, float _h) {

		GL::projection = glm::ortho(0.0f, _w, _h, 0.0f, -1.0f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(GL::genShader->program, "u_projection"), 1, GL_FALSE, &GL::projection[0][0]);
		glViewport(0, 0, _w, _h);
	}

	AvgGroup(int _x, int _y, int _w, int _h) : Object(x, y)
	{
		handleDraw = true;
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		w = _w;
		h = _h;
		x = _x;
		y = _y;

		initalW = _w;
		initalH = _h;

		ctb = new Texture(NULL, _w, _h);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctb->id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! " << _w << " " << _h << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	~AvgGroup()
	{
		if (ctb && !renderOnce)
			delete ctb;
	}

	void resize(float factor)
	{
		w = initalW * factor;
		h = initalH * factor;

		delete ctb;

		ctb = new Texture(NULL, w, h);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctb->id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! " << w << " " << h << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void forceDraw();

	virtual void draw();
};