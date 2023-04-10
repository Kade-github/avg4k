#include "Texture.h"

#include "imageinfo.hpp"
#include "stbi.h"
using namespace AvgEngine::OpenGL;

//Constructors and destructor

bool validImage(std::string path)
{
	std::ifstream file(path);
	if (file.bad())
	{
		AvgEngine::Logging::writeLog("[Texture] [Error] Failure to open " + path + " it does not exist.");
		return false;
	}
	file.close();
	const AvgEngine::External::ImageInfo imageInfo = AvgEngine::External::getImageInfo<AvgEngine::External::IIFilePathReader>(path.c_str());

	if (imageInfo.getErrorCode() != AvgEngine::External::II_ERR_OK || (imageInfo.getFormat() != AvgEngine::External::II_FORMAT_PNG && imageInfo.getFormat() != AvgEngine::External::II_FORMAT_JPEG))
	{
		AvgEngine::Logging::writeLog("[Texture] [Error] Failure to open file " + path + ", " + (imageInfo.getErrorMsg() != "Ok" ? std::string(imageInfo.getErrorMsg()) + ", " : "") + "Format: " + std::string(imageInfo.getExt()) + " (We only support JPEG/PNG's)");
		return false;
	}
	return true;
}

Texture* Texture::createWithImage(std::string filePath)
{
	if (!validImage(filePath))
	{
		unsigned char c[] = { 0, 0, 0, 255 };
		return new Texture(c, 1, 1);
	}
	Texture* t = External::stbi_h::stbi_load_file(filePath);
	t->fromSTBI = true;

	if (External::stbi_h::get_error())
	{
		unsigned char c[] = { 0, 0, 0, 255 };
		delete t;
		return new Texture(c, 1, 1);
	}

	return t;
}

Texture* Texture::loadTextureFromData(char* data, size_t outLength)
{
	return External::stbi_h::stbi_load_memory(data, outLength);
}

Texture* AvgEngine::OpenGL::Texture::loadTextureFromData(unsigned char* data, int w, int h)
{
	Texture* t = new Texture(data, w, h);
	return t;
}

texData Texture::getTextureData(std::string filePath)
{
	texData data{};
	data.w = 0;
	data.h = 0;
	data.data = External::stbi_h::stbi_load_file_data(filePath, &data.w, &data.h);
	return data;
}

void Texture::resizeTexture(int w, int h)
{
	glDeleteTextures(1, &id);
	glGenTextures(1, &id);
	SetData(pixels, w, h);
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

	glBindTexture(GL_TEXTURE_2D, id);
	glEnable(GL_TEXTURE_2D);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8,
		_width, _height, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}


Texture::~Texture()
{
	//Delete texture object
	glDeleteTextures(1, &id);
	if (pixels != nullptr && fromSTBI && width > 0 && height > 0)
		External::stbi_h::stbi_free(pixels);
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