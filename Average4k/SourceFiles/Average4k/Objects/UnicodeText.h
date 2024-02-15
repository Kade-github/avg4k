/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef AVG4K_UNICODE_TEXT_H
#define AVG4K_UNICODE_TEXT_H

#include <AvgEngine/External/SpriteFont/FntParser.h>
#include <AvgEngine/Base/GameObject.h>


namespace Average4k::Objects
{
	struct CharacterLine
	{
		AvgEngine::Render::Rect dst;
		AvgEngine::Render::Rect src;
		bool outline = false;
		bool space = false;
		int advance;
	};

	struct Line
	{
		int w;
		std::vector<CharacterLine> characters;
	};


	class UnicodeText : public AvgEngine::Base::GameObject
	{
	public:
		AvgEngine::Fnt::Fnt* fnt;

		bool wrap = false;

		float size = 0;
		float outlineThickness = 0;
		float characterSpacing = 0;

		bool centerLines = false;

		std::wstring text = L"";


		UnicodeText(float x, float y, std::string folder, std::string font, std::wstring _text, float _size) : AvgEngine::Base::GameObject(x, y)
		{
			if (folder.size() != 0 && font.size() != 0)
				SetFont(folder, font);
			SetSize(_size);
			SetText(_text);
			transform.w = _text.size() * size;
			transform.h = size;
			iTransform = transform;
			wrap = true;

		}

		void SetFont(std::string folder, std::string font)
		{
			// this doesn't actually reload the font's texture if it already existed.
			fnt = AvgEngine::Fnt::Fnt::GetFont(folder, font);
		}

		void SetSize(float _size)
		{
			size = _size;
		}

		void SetText(std::wstring _text)
		{
			text = _text;
		}

		void draw() override;
	};
}

#endif