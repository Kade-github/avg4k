#pragma once
#include "Sprite.h"
#include "Skin.h"
#include "Display.h"
namespace Average4k::Utils
{
	class Notifications : public GameObject
	{
		Skin* skin = NULL;
		bool visible = false;
	public:
		Sprite* errorIcon = NULL;
		Sprite* minorIcon = NULL;
		Sprite* button = NULL;

		Text* text = NULL;
		Text* title = NULL;
		Text* buttonText = NULL;

		AvgEngine::Render::Rect color;

		Notifications(float _x, float _y) : GameObject(_x,_y)
		{
			
		}

		void Show(std::string _ti, std::string _t, std::string _b, bool major = false, AvgEngine::Render::Rect c = { 0,0,0,0,240, 43, 43,1 })
		{
			color = c;
			text = new Text(0, 0, skin->GetFontPath(), "Arial.fnt", _t, 18);
			text->zIndex = 99;
			text->characterSpacing = 1.33f;
			title = new Text(0, 0, skin->GetFontPath(), "Arial.fnt", _ti, 18);
			title->characterSpacing = 2;
			title->zIndex = 99;
			buttonText = new Text(0, 0, skin->GetFontPath(), "Arial.fnt", _b, 18);
			buttonText->characterSpacing = 1.33f;
			buttonText->zIndex = 99;
			buttonText->transform.r = 0;
			buttonText->transform.g = 0;
			buttonText->transform.b = 0;

			text->centerLines = true;

			text->transform.r = 0;
			text->transform.g = 0;
			text->transform.b = 0;

			errorIcon->transform.a = 0;
			minorIcon->transform.a = 0;

			if (major)
				errorIcon->transform.a = 1;
			else
				minorIcon->transform.a = 1;

			button->transform.a = 1;

			addObject(text);
			addObject(title);
			addObject(buttonText);

			AvgEngine::Logging::writeLog("[Notification] Showing a notif with text: " + _t + ". Title: " + _ti);

			visible = true;
		}


		void Hide()
		{
			visible = false;
		}

		void Added() override
		{
			eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseDown, [&](AvgEngine::Events::Event e)
				{
					using namespace AvgEngine;
					Events::Vec vector = e.vector;
					
					if (!AvgEngine::Utils::Collision::AABB(vector.x, vector.y, 
						button->transform.x, button->transform.y, 
						button->transform.w, button->transform.h, static_cast<float>(Render::Display::width), static_cast<float>(
							Render::Display::height), false))
						return;
					Hide();
				}, false);
		}

		void InitNotifications(Skin* _skin)
		{
			delete errorIcon;
			delete minorIcon;
			delete button;

			skin = _skin;

			errorIcon = new Sprite(0, 0, skin->GetTexture("Menu/majorerroricon"));
			errorIcon->zIndex = 99;
			errorIcon->transform.w = 32;
			errorIcon->transform.h = 32;
			minorIcon = new Sprite(0, 0, skin->GetTexture("Menu/minorerroricon"));
			minorIcon->zIndex = 99;
			minorIcon->transform.w = 32;
			minorIcon->transform.h = 32;
			button = new Sprite(0, 0, skin->GetTexture("Menu/roundedbutton_ok"));
			button->zIndex = 99;
			addObject(errorIcon);
			addObject(minorIcon);
			addObject(button);

			errorIcon->transform.a = 0;
			minorIcon->transform.a = 0;
			button->transform.a = 0;
		}

		void draw() override
		{
			if (!text || !visible)
				return;

			

			float x = ((static_cast<float>(AvgEngine::Render::Display::width) / 2.0f) - 216.5f);
			float y = ((static_cast<float>(AvgEngine::Render::Display::height) / 2.0f) - 92.5f);
			float w = 433;
			float h = 185;

			transform.x = x;
			transform.y = y;
			transform.w = w;
			transform.h = h;

			AvgEngine::Render::Rect r = { x,y,w,h };
			r.r = 255;
			r.g = 255;
			r.b = 255;

			Primitives::DrawRectangle(camera, 99, r);
			r.h = 33;
			r.r = color.r;
			r.g = color.g;
			r.b = color.b;
			Primitives::DrawRectangle(camera, 99, r);

			errorIcon->transform.x = x;
			errorIcon->transform.y = y;
			minorIcon->transform.x = x;
			minorIcon->transform.y = y;

			title->transform.x = x + errorIcon->transform.w + 4;
			title->transform.y = y + ((errorIcon->transform.h / 2) - (title->transform.h / 2));
			text->transform.x = x + (w / 2);
			text->transform.y = y + (h / 2);
			text->wrap = true;

			button->transform.x = (x + w) - (button->transform.w + 8);
			button->transform.y = (y + h) - (button->transform.h + 8);

			buttonText->transform.x = button->transform.x + ((button->transform.w / 2) - (buttonText->transform.w / 2));
			buttonText->transform.y = button->transform.y + ((button->transform.h / 2) - (buttonText->transform.h / 2));

			if (text->transform.y + text->transform.h > button->transform.y)
				text->transform.y -= (text->transform.y + text->transform.h) - button->transform.y;

			GameObject::draw();
		}

	};
}