#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stbi.h"

Texture* stbi_h::stbi_load_file(std::string filePath)
{
	int w;
	int h;
	unsigned char* c = stbi_load(filePath.c_str(), &w, &h, nullptr, 4);
	return new Texture(c, w, h);
}

unsigned char* stbi_h::stbi_load_file_data(std::string filePath, int* w, int* h)
{
	unsigned char* c = stbi_load(filePath.c_str(), w, h, nullptr, 4);
	return c;
}

Texture* stbi_h::stbi_load_memory(char* memory, int size)
{
    int w = 0;
    int h = 0;

	unsigned char* data = stbi_load_from_memory((stbi_uc*)memory, size, &w, &h, NULL, 4);

    return new Texture(data, w, h);
}

void stbi_h::stbi_free(unsigned char* data)
{
	stbi_image_free(data);
}
