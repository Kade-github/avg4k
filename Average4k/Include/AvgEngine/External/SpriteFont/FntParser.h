/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef FNTPARSER_H
#define FNTPARSER_H

#pragma once

#include <AvgEngine/Render/Display.h>
#include <AvgEngine/External/XML/pugixml.hpp>
#include <AvgEngine/Render/OpenGL/Texture.h>

#include <AvgEngine/Utils/Logging.h>

#include <vector>

namespace AvgEngine::Fnt
{
	struct Kerning {
		int with = 0;
		int amount = 0;
	};

	struct FntChar
	{
		Render::Rect dst{};
		Render::Rect src{};
		int id = -1;
		int xAdvance = 0;
		std::vector<Kerning> kernings;

		bool operator<(const FntChar& a) const
		{
			return id < a.id;
		}
	};

	class Fnt
	{
	public:
		static std::vector<Fnt*>* fonts;
		std::vector<FntChar> chars{};
		std::string fontFile = "";
		std::string name = "";
		int ogSize = 0;

		bool hasKernings = false;

		std::shared_ptr<OpenGL::Texture> texture = NULL;

		static void ClearCache()
		{
			if (fonts->size() == 0)
			{
				#ifdef _DEBUG
				Logging::writeLog("[Fnt] [Debug] No fonts to clear.");
				#endif
				return;
			}
			#ifdef _DEBUG
			Logging::writeLog("[Fnt] [Debug] Clearing " + std::to_string(fonts->size()) + " fonts.");
			#endif
			for (Fnt* f : *fonts)
			{
				delete f;
			}
			fonts->clear();
			#ifdef _DEBUG
			Logging::writeLog("[Fnt] [Debug] Cleared successfully!");
			#endif
		}

		static Fnt* GetFont(std::string folder, std::string font)
		{
			for (Fnt* f : *fonts)
				if (f->fontFile == font)
					return f;
			#ifdef _DEBUG 
			Logging::writeLog("[Fnt] [Debug] First time load of " + font + ". Adding to cache...");
			#endif
			fonts->push_back(new Fnt(font, folder));
			for (Fnt* f : *fonts)
				if (f->fontFile == font)
					return f;
		}

		FntChar GetChar(char c)
		{
			unsigned int u = c;

			auto it = std::find_if(chars.begin(), chars.end(), [&u](const FntChar& obj) {return obj.id == u; });

			if (it != chars.end())
				return *it;
			
			FntChar cc;
			cc.id = -1;

			return cc;
		}

		FntChar* GetCharPtr(char c)
		{
			unsigned int u = c;

			for (FntChar& cc : chars)
				if (cc.id == u)
					return &cc;

			return NULL;
		}

		Fnt() = default;

		Fnt(std::string file, std::string folder)
		{
			fontFile = file;
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_file((folder + "/" + file).c_str());
			if (!result)
			{
				Logging::writeLog("[FNT] [Error] Failed to parse " + (folder + "/" + file));
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
				Logging::writeLog("[FNT] [Error] " + file + " doesn't have a info node.");
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
				Logging::writeLog("[FNT] [Error] " + file + " doesn't have a pages node.");
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
#ifdef _DEBUG
				Logging::writeLog("[Fnt] [Debug] Loaded " + std::to_string(chars.size()) + " characters.");
#endif
			}
			else
			{
				Logging::writeLog("[FNT] [Warning] " + file + " doesn't have a chars node.");
				return;
			}
			std::sort(chars.begin(), chars.end());

			n = doc.child("font").child("kernings");
			if (n != NULL)
			{
				int loaded = 0;
				for (pugi::xml_node c : n)
				{
					FntChar* cha = GetCharPtr(c.attribute("first").as_int());

					if (!cha)
					{
						Logging::writeLog("[FNT] [Kernings] [Warning] " + std::string(c.attribute("first").as_string()) + " doesn't exist!");
						continue;
					}

					Kerning k;
					k.with = c.attribute("second").as_int();
					k.amount = c.attribute("amount").as_int();
					cha->kernings.push_back(k);
					loaded++;
				}
				hasKernings = true;
				#ifdef _DEBUG
				Logging::writeLog("[Fnt] [Debug] Loaded " + std::to_string(loaded) + " kernings.");
				#endif
			}
			else
			{
				Logging::writeLog("[FNT] [Warning] " + file + " doesn't have a kernings node.");
				return;
			}
			
			Logging::writeLog("[Fnt] Loaded font " + name + "x" + std::to_string(ogSize));
		}
	};
}

#endif