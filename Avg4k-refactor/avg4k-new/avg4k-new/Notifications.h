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
			text->characterSpacing = 2.33f;
			title = new Text(0, 0, skin->GetFontPath(), "Arial.fnt", _ti, 18);
			title->characterSpacing = 3;
			buttonText = new Text(0, 0, skin->GetFontPath(), "Arial.fnt", _b, 14);
			buttonText->characterSpacing = 2.33f;

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
							Render::Display::height), true))
						return;
					Hide();
				});
		}

		void InitNotifications(Skin* _skin)
		{
			delete errorIcon;
			delete minorIcon;
			delete button;

			skin = _skin;

			errorIcon = new Sprite(0, 0, skin->GetTexture("Menu/majorerroricon"));
			minorIcon = new Sprite(0, 0, skin->GetTexture("Menu/minorerroricon"));
			button = new Sprite(0, 0, skin->GetTexture("Menu/roundedbutton_ok"));
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

			title->transform.x = x + errorIcon->transform.w;
			title->transform.y = y + 4;
			text->transform.x = x + (w / 2);
			text->transform.y = y + (h / 2);
			text->wrap = true;

			GameObject::draw();
		}

	};
}