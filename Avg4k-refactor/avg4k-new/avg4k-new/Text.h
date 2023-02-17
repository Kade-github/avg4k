#pragma once
#include "FntParser.h"
#include "includes.h"
#include "GameObject.h"
namespace AvgEngine::Base
{
	struct CharacterLine
	{
		Render::Rect dst;
		Render::Rect src;
		bool outline = false;
		bool space = false;
		int advance;
	};

	struct Line
	{
		int w;
		std::vector<CharacterLine> characters;
	};


	class Text : public GameObject
	{
	public:
		Fnt::Fnt* fnt;

		bool wrap = false;

		float size = 0;
		float outlineThickness = 0;
		float characterSpacing = 0;

		bool centerLines = false;

		std::string text = "";

		Text(int x, int y, std::string folder, std::string font, std::string _text, float _size) : GameObject(x,y)
		{
			if (folder.size() != 0 && font.size() != 0)
				SetFont(folder, font);
			SetSize(_size);
			SetText(_text);
		}

		~Text()
		{
			if (fnt)
				delete fnt;
			
		}

		void SetFont(std::string folder, std::string font)
		{
			// this doesn't actually reload the font's texture if it already existed.
			fnt = Fnt::Fnt::GetFont(folder, font);
		}

		void SetSize(float _size)
		{
			size = _size;
		}

		void SetText(std::string _text)
		{
			text = _text;
		}

		void draw() override
		{
			if (!fnt)
				return;
			Render::Rect dst = transform;
			if (transformRatio)
			{
				dst.x = parent->w * dst.x;
				dst.y = parent->h * dst.y;
			}

			drawCall call;
			call.texture = fnt->texture;
			call.shad = NULL;
			call.zIndex = zIndex;
			call.vertices = {};
			int totalW = 0;
			int highestH = 0;
			float scale = size / fnt->ogSize;
			std::vector<Line> lines;
			Line currentLine;
			int d = 0;
			for(char ch : text)
			{
				const Fnt::FntChar c = fnt->GetChar(ch);
				if (c.id == -1)
					continue;
				float advance = (c.xAdvance + characterSpacing) * scale;
				if (ch == 32)
				{
					CharacterLine l;
					l.space = true;
					l.advance = advance;
					totalW += advance;
					dst.x += advance;
					if (wrap && dst.x + dst.w > parent->x + parent->w && wrap)
					{
						dst.x = transform.x;
						dst.y += highestH;
						d = highestH + highestH;
						currentLine.w = totalW;
						lines.push_back(currentLine);
						currentLine = {};
						transform.w = totalW;
						totalW = 0;
					}

					currentLine.characters.push_back(l);
					continue;
				}
				Render::Rect src = c.src;
				dst.w = c.dst.w * scale;
				dst.h = c.dst.h * scale;


				if (highestH < dst.h)
					highestH = dst.h;

				CharacterLine l;
				l.dst = dst;
				l.src = src;
				l.advance = advance;

				if (outlineThickness != 0)
				{
					Render::Rect outline = dst;
					float outlineScale = (outlineThickness / 1);
					float nX = (dst.w * (1 - outlineScale)) / 2;
					outline.x += nX;
					outline.y += (dst.h * (1 - outlineScale)) / 2;
					outline.w *= outlineScale;
					outline.h *= outlineScale;
					outline.r = 0;
					outline.g = 0;
					outline.b = 0;
					CharacterLine ll;
					ll.dst = outline;
					ll.outline = true;
					ll.src = src;
					ll.advance = advance;
					currentLine.characters.push_back(ll);
				}
				currentLine.characters.push_back(l);
				dst.x += advance;
				totalW += advance;
			}
			currentLine.w = totalW;
			lines.push_back(currentLine);
			currentLine = {};


			if (d == 0)
				d = highestH;

			if (totalW > transform.w)
				transform.w = totalW;

			for(Line& line : lines)
			{
				int newStartX = transform.x;
				if (centerLines)
					newStartX = transform.x - (line.w / 2);

				int currentAdvance = 0;

				for(CharacterLine& c : line.characters)
				{
					int newStartY = c.dst.y;
					if (centerLines)
						newStartY = c.dst.y - (d / 2);
					c.dst.x = newStartX + currentAdvance;
					c.dst.y = newStartY;
					if (!c.space)
					{
						drawCall ca = Camera::FormatDrawCall(zIndex, fnt->texture, NULL, Render::DisplayHelper::RectToVertex(c.dst, c.src));
						camera->addDrawCall(ca);
					}
					if (!c.outline)
						currentAdvance += c.advance;
				}
			}


			transform.h = d;

			camera->addDrawCall(call);

			GameObject::draw();
		}
	};
}