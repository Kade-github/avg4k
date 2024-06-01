/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <AvgEngine/External/Image/stbi.h>
#include <AvgEngine/Utils/Logging.h>

using namespace AvgEngine::External;

std::shared_ptr<AvgEngine::OpenGL::Texture> stbi_h::stbi_load_file(std::string filePath)
{
	int w;
	int h;
	unsigned char* c = stbi_load(filePath.c_str(), &w, &h, nullptr, 4);
	return std::make_shared<AvgEngine::OpenGL::Texture>(c, w, h);
}

unsigned char* stbi_h::stbi_load_file_data(std::string filePath, int* w, int* h)
{
	unsigned char* c = stbi_load(filePath.c_str(), w, h, nullptr, 4);
	return c;
}

std::shared_ptr<AvgEngine::OpenGL::Texture> stbi_h::stbi_load_memory(char* memory, int size)
{
    int w = 0;
    int h = 0;

	unsigned char* data = stbi_load_from_memory((stbi_uc*)memory, size, &w, &h, NULL, 4);

	if (get_error())
		AvgEngine::Logging::writeLog("[Image] [Error] [Memory] failure to load " + std::string(stbi_failure_reason()));

    return std::make_shared<AvgEngine::OpenGL::Texture>(data, w, h);
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
