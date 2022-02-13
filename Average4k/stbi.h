#pragma once
#include "Texture.h"
struct Pixel {
	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;
};

class stbi_h
{
public:
	static Texture* stbi_load_file(std::string filePath);
	static Texture* stbi_load_memory(char* memory, int size);
	static void stbi_free(unsigned char* data);
	static Pixel getPixelFromImage(unsigned char* data, int width, int x, int y)
	{
		unsigned char* pp = data + (4 * (y * width + x));

		Pixel p;
		p.r = (int)pp[0];
		p.g = (int)pp[1];
		p.b = (int)pp[2];
		p.a = (int)pp[3];
		return p;
	}
	static Pixel getLightestPixel(unsigned char* data, int width, int height)
	{
		Pixel lightest;
		int x = 0, y = 0;
		for (int i = 0; i < width; i++)
		{
			x = i;
			for (int ii = 0; ii < height; ii++)
			{
				y = ii;
				Pixel pixel = getPixelFromImage(data, width, x, y);
				if (pixel.r >= 240 && pixel.g >= 240 && pixel.b >= 240 || pixel.a == 0)
					continue;
				if (pixel.r > lightest.r && pixel.g > lightest.g && pixel.b > lightest.b && pixel.a >= lightest.a)
					lightest = pixel;
			}
		}
		return lightest;
	}
	static Pixel getDarkestPixel(unsigned char* data, int width, int height)
	{
		Pixel lightest = getLightestPixel(data,width,height);
		Pixel darkest;
		int x = 0, y = 0;
		for (int i = 0; i < width; i++)
		{
			x = i;
			for (int ii = 0; ii < height; ii++)
			{
				y = ii;
				Pixel pixel = getPixelFromImage(data, width, x, y);
				if (pixel.r < lightest.r && pixel.g < lightest.g && pixel.b < lightest.b && pixel.a <= lightest.a)
					darkest = pixel;
			}
		}
		return darkest;
	}
	static Pixel getAccentPixel(unsigned char* data, int width, int height, Pixel toCompare, Pixel toCompare2)
	{
		int x = 0, y = 0;
		Pixel accent = toCompare;
		for (int i = 0; i < width; i++)
		{
			x = i;
			for (int ii = 0; ii < height; ii++)
			{
				y = ii;
				Pixel pixel = getPixelFromImage(data, width, x, y);
				if (pixel.r <= 100 || pixel.g <= 100 || pixel.b <= 100 || pixel.a == 0)
					continue;
				if ((pixel.r < toCompare.r && pixel.g < toCompare.g && pixel.b < toCompare.b)
					&&
					(pixel.r > toCompare2.r && pixel.g > toCompare2.g && pixel.b > toCompare2.b))
					accent = pixel;
			}
		}
		return accent;
	}
};