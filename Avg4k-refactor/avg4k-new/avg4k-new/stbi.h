#pragma once
#include "Texture.h"
namespace AvgEngine::External
{
	struct Pixel {
		int r = 0;
		int g = 0;
		int b = 0;
		int a = 0;
	};

	/**
	 * \brief A helper class to interface with stbi_image
	 */
	class stbi_h
	{
	public:
		/**
		 * \brief Loads a texture from an image
		 * \param filePath The file path
		 * \return The loaded texture
		 */
		static OpenGL::Texture* stbi_load_file(std::string filePath);
		/**
		 * \brief Loads the file data for a given image
		 * \param filePath The file path
		 * \param w A reference to a variable to output the width to
		 * \param h A reference to a variable to output the height to
		 * \return The image data
		 */
		static unsigned char* stbi_load_file_data(std::string filePath, int* w, int* h);
		/**
		 * \brief Loads a texture from memory
		 * \param memory The memory to load from
		 * \param size The size of the memory
		 * \return The loaded texture
		 */
		static OpenGL::Texture* stbi_load_memory(char* memory, int size);
		/**
		 * \brief Frees stbi_image loaded data
		 * \param data The STBI loaded data to free
		 */
		static void stbi_free(unsigned char* data);
		/**
		 * \brief Returns true or false if an error code exists
		 * \return Boolean
		 */
		static bool get_error();

		/**
		 * \brief A function to obtain a pixel's color values from a data stream
		 * \param data The image data to scan
		 * \param width The width of the image
		 * \param x The x coordinate of the image to obtain
		 * \param y The y coordinate of the image to obtain
		 * \return A pixel struct with the data requested
		 */
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

		/**
		 * \brief A helper function to obtain the lightest pixel in a data stream
		 * \param data The image data to scan
		 * \param width The width of the image
		 * \param height The height of the image
		 * \return The lightest pixel as a Pixel Struct
		 */
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

		/**
		 * \brief A helper function to obtain the darkest pixel in a data stream
		 * \param data The image data to scan
		 * \param width The width of the image
		 * \param height The height of the image
		 * \return The darkest pixel as a Pixel Struct
		 */
		static Pixel getDarkestPixel(unsigned char* data, int width, int height)
		{
			Pixel lightest = getLightestPixel(data, width, height);
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

		/**
		 * \brief A helper function to compare two pixels to generate an accent pixel
		 * \param data The image data to scan
		 * \param width The width of the image
		 * \param height The height of the image
		 * \param toCompare The first pixel to compare
		 * \param toCompare2 The second pixel to compare
		 * \return The accent pixel as a Pixel Struct
		 */
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
}