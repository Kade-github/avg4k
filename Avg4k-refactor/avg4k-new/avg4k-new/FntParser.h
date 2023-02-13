#pragma once
#include "includes.h"
#include "pugixml.hpp"
#include "Texture.h"

namespace AvgEngine::Fnt
{
	struct FntChar
	{
		Render::Rect dst{};
		Render::Rect src{};
		int id = 0;
		int xAdvance = 0;
	};

	class Fnt
	{
	public:
		std::vector<FntChar> chars{};
		std::string fontFile = "";
		std::string name = "";
		int ogSize = 0;

		OpenGL::Texture* texture = NULL;

		static Fnt* GetFont(std::string folder, std::string font)
		{
			static std::vector<Fnt*>* fonts;

			if (fonts == NULL)
				fonts = new std::vector<Fnt*>();
			for (Fnt* f : *fonts)
				if (f->fontFile == font)
					return f;
			fonts->push_back(new Fnt(font, folder));
			for (Fnt* f : *fonts)
				if (f->fontFile == font)
					return f;
		}

		~Fnt()
		{
			if (texture)
				if (!texture->dontDelete)
					delete texture;
		}

		FntChar GetChar(char c)
		{
			unsigned int u = c;

			for (FntChar cc : chars)
				if (cc.id == u)
					return cc;

			FntChar cc;
			cc.id = -1;

			return cc;
		}

		Fnt() = default;

		Fnt(std::string file, std::string folder)
		{
			fontFile = file;
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_file((folder + "/" + file).c_str());
			if (!result)
			{
				Logging::writeLog("[Error] [FNT] Failed to parse " + (folder + "/" + file));
				return;
			}

			pugi::xml_node n = doc.child("font").child("info");

			if (n != NULL)
			{
				name = n.attribute("face").as_string();
				ogSize = n.attribute("size").as_int();
			}
			else
			{
				Logging::writeLog("[Error] [FNT] " + file + " doesn't have a info node.");
				return;
			}

			n = doc.child("font").child("pages");
			if (n != NULL)
			{
				std::string toLoad = folder + "/" + n.first_child().attribute("file").as_string();
				texture = OpenGL::Texture::createWithImage(toLoad);
			}
			else
			{
				Logging::writeLog("[Error] [FNT] " + file + " doesn't have a pages node.");
				return;
			}

			n = doc.child("font").child("chars");
			if (n != NULL)
			{
				for (pugi::xml_node c : n)
				{
					FntChar ch;
					ch.xAdvance = c.attribute("xadvance").as_int();
					ch.id = c.attribute("id").as_int();

					float x = c.attribute("x").as_float();
					float y = c.attribute("y").as_float();
					float w = c.attribute("width").as_float();
					float h = c.attribute("height").as_float();

					ch.dst = { x, y, w, h };

					ch.src = { x / texture->width, y / texture->height,w / texture->width, h / texture->height };
					chars.push_back(ch);
				}
			}
			else
			{
				Logging::writeLog("[Error] [FNT] " + file + " doesn't have a chars node.");
				return;
			}
			
			Logging::writeLog("[Fnt] Loaded font " + name + "x" + std::to_string(ogSize));
		}
	};
}
