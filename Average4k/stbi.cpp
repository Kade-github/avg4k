#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stbi.h"

Texture* stbi_h::stbi_load_file(std::string filePath)
{
	int w;
	int h;
	unsigned char* c = stbi_load(filePath.c_str(), &w, &h, nullptr, 4);
	if (stbi_failure_reason())
		std::cout << "STBI: FAILURE " << stbi_failure_reason() << " ON " << filePath << std::endl;
	return new Texture(c, w, h);
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
