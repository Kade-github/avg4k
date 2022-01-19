#pragma once
#include "Texture.h"
class stbi_h
{
public:
	static Texture* stbi_load_file(std::string filePath);
	static Texture* stbi_load_memory(char* memory, int size);
	static void stbi_free(unsigned char* data);
};