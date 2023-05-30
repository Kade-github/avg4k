#pragma once
#include "LuaGameObject.h"
#include "Text.h"
namespace Average4k::Lua::Base
{
	struct textObject : public gameObject {
	public:

		float spacing = 0;
		float size = 12;
		float outline = 0;
		bool wrap = false;
		bool center = false;

		std::string font = "";
		std::string text = "";

		void setSize(float _size)
		{
			size = _size;
			if (base)
			{
				AvgEngine::Base::Text* te = static_cast<AvgEngine::Base::Text*>(base);
				te->SetSize(_size);
				transform.w = base->transform.w;
				transform.h = base->transform.h;
			}
		}

		float getSize()
		{
			return size;
		}

		void setOutline(float _outline)
		{
			outline = _outline;
			if (base)
			{
				AvgEngine::Base::Text* te = static_cast<AvgEngine::Base::Text*>(base);
				te->outlineThickness = outline;
			}
		}

		float getOutline()
		{
			return outline;
		}

		void setSpacing(float _spacing)
		{
			spacing = _spacing;
			if (base)
			{
				AvgEngine::Base::Text* te = static_cast<AvgEngine::Base::Text*>(base);
				te->characterSpacing = spacing;
				transform.w = base->transform.w;
				transform.h = base->transform.h;
			}
		}

		bool getSpacing()
		{
			return spacing;
		}


		void setWrapped(bool _wrap)
		{
			wrap = _wrap;
			if (base)
			{
				AvgEngine::Base::Text* te = static_cast<AvgEngine::Base::Text*>(base);
				te->wrap = wrap;
			}
		}

		bool getWrapped()
		{
			return wrap;
		}

		void setCenter(bool _center)
		{
			center = _center;
			if (base)
			{
				AvgEngine::Base::Text* te = static_cast<AvgEngine::Base::Text*>(base);
				te->centerLines = center;
			}
		}

		bool getCentered()
		{
			return center;
		}

		void setFont(std::string _font);
		void setText(std::string _text)
		{
			text = _text;
			if (base)
			{
				AvgEngine::Base::Text* te = static_cast<AvgEngine::Base::Text*>(base);
				te->SetText(text);
				transform.w = base->transform.w;
				transform.h = base->transform.h;
			}
		}

		std::string getFont()
		{
			return font;
		}

		std::string getText()
		{
			return text;
		}

		textObject(float x, float y, std::string _font, std::string _text) : gameObject(x, y)
		{
			type = 2;
			font = _font;
			text = _text;
		}
	};
}