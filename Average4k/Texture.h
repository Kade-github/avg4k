#pragma once
#include "Includes.h"

struct texData
{
	unsigned char* data;
	int w, h;
};

class Texture
{
	public:
		//GL object
		GLuint id;

		SDL_Surface* surf;

		//Constructors and destructor

		bool dontDelete = false;

		static Texture* createFromSurface(SDL_Surface* surf, bool free);

		static Texture* createWithImage(std::string filePath);

		static Texture* loadTextureFromData(unsigned char* data, int w, int h);

		static texData getTextureData(std::string filePath);

		void resizeTexture(int w, int h);

		int width;
		int height;
		bool fromSTBI = false;
		unsigned char* pixels;

		Texture(unsigned char* data, const unsigned int width, const unsigned int height);
		Texture(unsigned char* data, const unsigned int width, const unsigned int height, bool mssa);
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

