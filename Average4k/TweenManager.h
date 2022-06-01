#pragma once
#include "pch.h"
#include "Object.h"
#include "Easing.h"
#include "AvgButton.h"
#include "GL.h"

namespace Tweening
{
	typedef void (__cdecl *tweenCallback)();
	enum TweenType {
		tt_Alpha = 0, // alpha tween
		tt_X = 1, // just x tween
		tt_Y = 2, // just y tween
		tt_butFill = 3,
		tt_scale = 4,
	};

	struct Tween {
		tweenCallback callback;
		std::string name;
		bool call;
		double duration;
		double time;
		double percnt;
		Object* obj;
		TweenType type;
		double end;
		int savedId;
		bool shouldBeId;
		double start;
		Easing::easingFunction easeFunc;
		std::map<std::string, int> vars;

		friend bool operator==(const Tween& lhs, const Tween& rhs)
		{
			return lhs.name == rhs.name;
		}
	};

	class TweenManager {
	public:

		static std::vector<Tweening::Tween> tweenRemove;
		static std::vector<Tween> activeTweens;
		static void removeTween(std::string ident)
		{
			for (int i = 0; i < activeTweens.size(); i++)
			{
				Tween& t = activeTweens[i];
				if (t.name == ident && !t.call)
				{
					switch (t.type)
					{
					case tt_Alpha:
						t.obj->alpha = t.end;
						break;
					case tt_X:
						t.obj->setX(t.end);
						break;
					case tt_Y:
						t.obj->setY(t.end);
						break;
					case tt_scale:
						t.obj->scale = t.end;
						break;
					case tt_butFill:
						AvgButton* but = (AvgButton*)t.obj;
						if (t.end == 1)
						{
							but->fillColor.r = but->defColor.r;
							but->fillColor.g = but->defColor.g;
							but->fillColor.b = but->defColor.b;
						}
						else
						{
							but->fillColor.r = but->hoverColor.r;
							but->fillColor.g = but->hoverColor.g;
							but->fillColor.b = but->hoverColor.b;
						}
						break;
					}
					t.call = true;
					activeTweens.erase(std::remove(activeTweens.begin(), activeTweens.end(), t), activeTweens.end());
				}
			}
		}
		static Tween createNewTween(std::string identity, Object* toTween, TweenType type, double dur, double s, double e, tweenCallback callbackFunc, Easing::easing_functions easeType, bool shouldBeIdd = true)
		{
			//std::cout << "creating " << identity << std::endl;
			for (int i = 0; i < activeTweens.size(); i++)
			{
				Tween& t = activeTweens[i];
				if (t.name == identity && !t.call)
				{
					std::cout << "remove tween cuz its already goin on" << std::endl;
					switch (t.type)
					{
					case tt_Alpha:
						t.obj->alpha = t.end;
						break;
					case tt_X:
						t.obj->setX(t.end);
						break;
					case tt_Y:
						t.obj->setY(t.end);
						break;
					case tt_scale:
						t.obj->scale = t.end;
						break;
					case tt_butFill:
						AvgButton* but = (AvgButton*)t.obj;
						if (t.end == 1)
						{
							but->fillColor.r = but->defColor.r;
							but->fillColor.g = but->defColor.g;
							but->fillColor.b = but->defColor.b;
						}
						else
						{
							but->fillColor.r = but->hoverColor.r;
							but->fillColor.g = but->hoverColor.g;
							but->fillColor.b = but->hoverColor.b;
						}
						break;
					}
					t.call = true;
					activeTweens.erase(std::remove(activeTweens.begin(), activeTweens.end(), t), activeTweens.end());
				}
			}
			Tween tw;
			tw.name = identity;
			tw.duration = dur;
			tw.type = type;
			tw.obj = toTween;
			tw.end = e;
			tw.savedId = toTween->id;
			tw.shouldBeId = shouldBeIdd;
			tw.call = false;
			tw.start = s;
			Easing::easingFunction ease = Easing::getEasingFunction(easeType);
			tw.easeFunc = ease;
			tw.callback = callbackFunc;
			tw.time = 0;
			tw.percnt = 0;

			if (tw.type == tt_scale)
			{
				tw.vars["w"] = tw.obj->w;
				tw.vars["h"] = tw.obj->h;
			}

			activeTweens.push_back(tw);
			return tw;
		}

		static void updateTween(Tween& t, float elapsed)
		{
			t.time += elapsed;
			t.percnt = t.time / t.duration;
			if (t.percnt > 1)
				t.percnt = 1;
			double start = t.start;
			double end = t.end;
			double value = t.easeFunc(t.percnt);

			if (!Game::doesObjectExist(t.savedId) && t.shouldBeId)
			{
				tweenRemove.push_back(t);
				return;
			}

			if (t.obj->w < 0)
			{
				tweenRemove.push_back(t);
				return;
			}
			
			if (t.percnt >= 1.0)
			{
				// finished
				tweenRemove.push_back(t);
				if (t.callback != nullptr && !t.call)
				{
					t.call = true;
					t.callback();
				}

			}

			switch (t.type)
			{
			case tt_Alpha:
				t.obj->setAlpha(start + ((end - start) * value));
				break;
			case tt_X:
				t.obj->setX(start + ((end - start) * value));
				break;
			case tt_Y:
				t.obj->setY(start + ((end - start) * value));
				break;
			case tt_scale:
				t.obj->scale = start + ((end - start) * value);
				break;
			}
		}
	};

	class AvgButtonTweenable : public AvgButton
	{
	public:

		Texture* button;
		Texture* select;

		AvgButtonTweenable(float _x, float _y, int _w, int _h, std::string _text, int fontSize, std::string font, clickCallback _callback) : AvgButton(_x, y, _w, _h, _text, fontSize, font, callback) {
			x = _x;
			y = _y;
			w = _w;
			h = _h;
			text = new Text(x, y, _text, fontSize, font);
			text->create();
			callback = _callback;
			add(text);
			button = Noteskin::getMenuElement(Game::noteskin, "MainMenu/button.png");
			select = Noteskin::getMenuElement(Game::noteskin, "MainMenu/button_selected.png");
		};

		virtual ~AvgButtonTweenable()
		{
			text->beforeDeath();
			text->die();
		}

		void clicked()
		{
			if (callback != NULL && hovered)
				callback();
		}
	
		void mouseDown()
		{
			clicked();
		}

		void draw() {
			int mx, my;
			Game::GetMousePos(&mx, &my);

			if (!mouse)
			{
				if (selected && !hovered)
				{
					hovered = true;
				}
				else if (!selected && hovered)
				{
					hovered = false;
				}
			}

			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				mouse = true;
				hovered = true;
				selected = false;
			}
			else
			{
				if (!selected)
					hovered = false;
			}

			Rect rect;


			float mpx = (w * (1 - scale)) / 2;
			float mpy = (h * (1 - scale)) / 2;

			float scaledX = x + mpx;
			float scaledY = y + mpy;
			float scaledWidth = w * scale;
			float scaledHeight = h * scale;

			rect.x = scaledX;
			rect.y = scaledY;
			if (!staticView)
			{
				rect.x -= Game::mainView.x;
				rect.y -= Game::mainView.y;
			}
			rect.w = scaledWidth;
			rect.h = scaledHeight;
			rect.r = fillColor.r;
			rect.g = fillColor.g;
			rect.b = fillColor.b;
			rect.a = alpha;

			Rect srcRect;
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = 1;
			srcRect.h = 1;

			if (text->text != "")
			{
				if (text->color.r != fontColor.r || text->color.g != fontColor.g || text->color.b != fontColor.b)
				{
					text->color = fontColor;
					text->setText(text->text);
				}
				text->setX((scaledX + (scaledWidth / 2)) - (text->surfW / 2));
				text->setY((scaledY + (text->surfH / 2)) + 5);
				if (alpha != text->alpha)
					text->setAlpha(alpha);
			}


			if (hovered)
				Rendering::PushQuad(&rect, &srcRect, select, GL::genShader);
			else
				Rendering::PushQuad(&rect, &srcRect, button, GL::genShader);
		}
	};
}