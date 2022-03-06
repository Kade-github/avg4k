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

	void bind(float w, float h) {

		GL::projection = glm::ortho(0.0f, w, h, 0.0f, -1.0f, 1.0f);

		glUniformMatrix4fv(glGetUniformLocation(GL::genShader->program, "u_projection"), 1, GL_FALSE, &GL::projection[0][0]);
		glViewport(0, 0, w, h);
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

	void forceDraw();

	virtual void draw();
};