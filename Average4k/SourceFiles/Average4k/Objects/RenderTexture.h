/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once
#include <AvgEngine/Render/OpenGL/Texture.h>
#include <AvgEngine/Utils/Logging.h>
#include <AvgEngine/Render/Display.h>

#include <AvgEngine/Base/Camera.h>

#ifndef AVG4K_RENDER_TEXTURE_H
#define AVG4K_RENDER_TEXTURE_H

namespace Average4k::Objects
{

	class RenderTexture
	{
	public:
		GLuint fb;
		std::shared_ptr<AvgEngine::OpenGL::Texture> texture = NULL;

		AvgEngine::Base::Camera* camera = NULL;

		int _w, _h;

		RenderTexture(AvgEngine::Base::Camera* cam, int w, int h) : _w(w), _h(h)
		{
			camera = cam;

			glGenFramebuffers(1, &fb);
			glBindFramebuffer(GL_FRAMEBUFFER, fb);

			unsigned char* data = new unsigned char[w * h * 4];

			texture = std::make_shared<AvgEngine::OpenGL::Texture>(data, w, h);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				AvgEngine::Logging::writeLog("[RenderTexture] [Error] Framebuffer not complete.");
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		~RenderTexture()
		{
			glDeleteFramebuffers(1, &fb);
		}

		void Resize(int w, int h)
		{
			texture->resizeTexture(w, h);
			glBindFramebuffer(GL_FRAMEBUFFER, fb);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Bind()
		{
			camera->draw();
			glBindFramebuffer(GL_FRAMEBUFFER, fb);
			glColorMask(TRUE, TRUE, TRUE, TRUE);
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT);

			glViewport(0, 0, _w, _h);
			AvgEngine::Render::Display::defaultShader->setProject(glm::ortho(0.0f, (float)_w, (float)_h, 0.0f));
		}

		void Unbind()
		{
			camera->w = _w;
			camera->h = _h;
			camera->draw();
			camera->w = AvgEngine::Render::Display::width;
			camera->h = AvgEngine::Render::Display::height;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glViewport(0, 0, AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);
			AvgEngine::Render::Display::defaultShader->setProject(glm::ortho(0.0f, (float)AvgEngine::Render::Display::width, (float)AvgEngine::Render::Display::height, 0.0f));
		}

	};
}
#endif