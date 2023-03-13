#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stbi.h"

using namespace AvgEngine::External;

AvgEngine::OpenGL::Texture* stbi_h::stbi_load_file(std::string filePath)
{
	int w;
	int h;
	unsigned char* c = stbi_load(filePath.c_str(), &w, &h, nullptr, 4);
	return new AvgEngine::OpenGL::Texture(c, w, h);
}

unsigned char* stbi_h::stbi_load_file_data(std::string filePath, int* w, int* h)
{
	unsigned char* c = stbi_load(filePath.c_str(), w, h, nullptr, 4);
	return c;
}

AvgEngine::OpenGL::Texture* stbi_h::stbi_load_memory(char* memory, int size)
{
    int w = 0;
    int h = 0;

	unsigned char* data = stbi_load_from_memory((stbi_uc*)memory, size, &w, &h, NULL, 4);

	if (get_error())
		AvgEngine::Logging::writeLog("[Image] [Error] failure to load " + std::string(stbi_failure_reason()));

    return new AvgEngine::OpenGL::Texture(data, w, h);
}

void stbi_h::stbi_free(unsigned char* data)
{
	stbi_image_free(data);
}

bool stbi_h::get_error()
{
	if (stbi_failure_reason())
	{
		std::string reason = std::string(stbi_failure_reason());
		if (reason != "can't fopen" && reason != "bad png sig")
		{
			return true;
		}
	}
	return false;
}
