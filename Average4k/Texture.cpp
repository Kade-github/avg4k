#include "stbi.h"
#include "Rendering.h"
#include "Game.h"
//Constructors and destructor

// png file header 137 80 78 71 13 10 26 10

bool validPng(std::string path)
{
	std::fstream s(path);
	if (s.bad())
		return false;

	// read the data
	char buff[8 + 1]{ 0 };
	s.read(buff, 8);

	std::string st = std::string(buff);

	if (!st.starts_with("‰PNG"))
		return false;
	return true;
}

Texture* Texture::createWithImage(std::string filePath)
{
	if (!validPng(filePath))
	{
		unsigned char c[] = { 0, 0, 0, 255 };
		return new Texture(c, 1, 1);
	}
	Texture* t = stbi_h::stbi_load_file(filePath);
	t->fromSTBI = true;

	if (stbi_h::get_error())
	{
		unsigned char c[] = { 0, 0, 0, 255 };
		delete t;
		return new Texture(c, 1, 1);
	}

	return t;
}

Texture* Texture::loadTextureFromData(unsigned char* data, int w, int h)
{
	return new Texture(data, w, h);
}

texData Texture::getTextureData(std::string filePath)
{
	texData data;
	data.w = 0;
	data.h = 0;
	data.data = stbi_h::stbi_load_file_data(filePath, &data.w, &data.h);
	return data;
}

void Texture::resizeTexture(int w, int h)
{
	glDeleteTextures(1, &id);
	glGenTextures(1, &id);
	SetData(pixels, w, h);
}

Texture* Texture::createFromSurface(SDL_Surface* surf, bool free) {
	if (surf == NULL)
	{
		return Rendering::white;
	}
	int w = surf->w;
	int h = surf->h;
	unsigned char* c = (unsigned char*)surf->pixels;
	Texture* t = new Texture(c, w, h);
	t->surf = surf;
	return t;
}

Texture::Texture(unsigned char* data, const unsigned int width, const unsigned int height)
{
	//Create texture object and use given data
	glGenTextures(1, &id);
	SetData(data, width, height);
}

Texture::Texture(unsigned char* data, const unsigned int _width, const unsigned int _height, bool mssa)
{
	pixels = data;
	width = _width;
	height = _height;

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
	glEnable(GL_TEXTURE_2D_MULTISAMPLE);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, _width, _height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}


Texture::~Texture()
{
	//Delete texture object
	glDeleteTextures(1, &id);
	if (pixels != nullptr && fromSTBI && width > 0 && height > 0)
		stbi_h::stbi_free(pixels);
	if (surf && !fromSTBI)
	{
		SDL_FreeSurface(surf);
	}
}
		
//Set texture data function
bool Texture::SetData(unsigned char* data, const unsigned int _width, const unsigned int _height)
{
	pixels = data;
	width = _width;
	height = _height;

	glBindTexture(GL_TEXTURE_2D, id);
	glEnable(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Game::instance->createTexture(this);
			
	return true;
}