#pragma once
#include "Sprite.h"
#include "Skin.h"
#include "Display.h"
namespace Average4k::Utils
{
	class Notifications : public GameObject
	{
		Skin* skin;
		bool visible = false;
	public:
		Sprite* errorIcon = NULL;
		Sprite* minorIcon = NULL;
		Sprite* button = NULL;

		Text* text;
		Text* title;
		Text* buttonText;

		Notifications(float _x, float _y) : GameObject(_x,_y)
		{
			
		}

		void Show(std::string _ti, std::string _t, std::string _b)
		{
			text = new Text(0, 0, skin->GetFontPath(), "Arial.fnt", _t, 14);
			text->characterSpacing = 2.33f;
			title = new Text(0, 0, skin->GetFontPath(), "Arial.fnt", _ti, 18);
			title->characterSpacing = 3;
			buttonText = new Text(0, 0, skin->GetFontPath(), "Arial.fnt", _ti, 14);
			buttonText->characterSpacing = 2.33f;

			addObject(text);
			addObject(title);
			addObject(buttonText);

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
						button->transform.w, button->transform.h, true))
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
			minorIcon = new Sprite(0, 0, skin->GetTexture("Menu/minorIcon"));
			button = new Sprite(0, 0, skin->GetTexture("Menu/roundedbutton_ok"));
			addObject(errorIcon);
			addObject(minorIcon);
			addObject(button);

			errorIcon->transform.a = 0;
			errorIcon->transformRatio = true;
			minorIcon->transform.a = 0;
			minorIcon->transformRatio = true;
			button->transform.a = 0;
			button->transformRatio = true;
		}

		void draw() override
		{
			if (!text || !visible)
				return;

			GameObject::draw();
		}

	};
}