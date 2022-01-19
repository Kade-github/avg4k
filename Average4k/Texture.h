#pragma once
#include "Includes.h"

class Texture
{
	public:
		//GL object
		GLuint id;

		//Constructors and destructor

		static Texture* createFromSurface(SDL_Surface* surf, bool free);

		static Texture* createWithImage(std::string filePath);

		int width;
		int height;
		bool fromSTBI = false;
		unsigned char* pixels;

		Texture(unsigned char* data, const unsigned int width, const unsigned int height);
		~Texture();
				
		//Texture interface
		bool SetData(unsigned char* data, const unsigned int width, const unsigned int height);
		bool Bind()
		{
			glBindTexture(GL_TEXTURE_2D, id);
			return false;
		}
		bool Unbind()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			return false;
		}
};

