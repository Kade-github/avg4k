/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once

#include <Glad/glad.h>
#include <iostream>

namespace AvgEngine::OpenGL
{
	struct texData
	{
		unsigned char* data;
		int w, h;
	};

	/**
	 * \brief The OpenGL Texture Class
	 */
	class Texture
	{
	public:
		//GL object
		GLuint id{};

		//Constructors and destructor

		bool dontDelete = false;

		static Texture* returnWhiteTexture()
		{
			static Texture* t = NULL;
			if (t == NULL)
			{
				unsigned char c[] = { 255, 255, 255, 255 };
				t = new Texture(c, 1, 1);
				t->dontDelete = true;
			}
			return t;
		}

		static Texture* createWithImage(std::string filePath);

		static Texture* loadTextureFromLoadedData(unsigned char* data, int w, int h)
		{
			return new Texture(data, w, h);
		}
		static Texture* loadTextureFromData(char* data, size_t size);
		static Texture* loadTextureFromData(unsigned char* data, int w, int h);

		static texData getTextureData(std::string filePath);

		void resizeTexture(int w, int h);

		int width{};
		int height{};
		bool fromSTBI = false;
		unsigned char* pixels{};

		Texture(unsigned char* data, const unsigned int width, const unsigned int height);
		Texture(unsigned char* data, const unsigned int width, const unsigned int height, bool mssa);
		~Texture();

		//Texture interface
		bool SetData(unsigned char* data, const unsigned int width, const unsigned int height);
		bool Bind();
		bool Unbind();
	};


}

#endif // !TEXTURE_H