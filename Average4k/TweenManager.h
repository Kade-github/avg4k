#pragma once
#include "pch.h"
#include "Object.h"
#include "Easing.h"

namespace Tweening
{
	typedef void*(*tweenCallback)();
	enum TweenType {
		tt_Alpha = 0, // alpha tween
		tt_X = 1, // just x tween
		tt_Y = 2 // just y tween
	};

	struct Tween {
		tweenCallback callback;
		std::string name;
		double duration;
		double time;
		double percnt;
		Object* obj;
		TweenType type;
		double end;
		double start;
		Easing::easingFunction easeFunc;

		friend bool operator==(const Tween& lhs, const Tween& rhs)
		{
			return lhs.name == rhs.name;
		}
	};

	class TweenManager {
	public:
		static std::vector<Tween> activeTweens;
		static Tween createNewTween(std::string identity, Object* toTween, TweenType type, double dur, double s, double e, tweenCallback callbackFunc, Easing::easing_functions easeType)
		{
			for (int i = 0; i < activeTweens.size(); i++)
			{
				Tween& t = activeTweens[i];
				if (t.name == identity)
				{
					if (t.callback != nullptr)
						t.callback();
					activeTweens.erase(std::remove(activeTweens.begin(), activeTweens.end(), t), activeTweens.end());
				}
			}
			Tween tw;
			tw.name = identity;
			tw.duration = dur;
			tw.type = type;
			tw.obj = toTween;
			tw.end = e;
			tw.start = s;
			Easing::easingFunction ease = Easing::getEasingFunction(easeType);
			tw.easeFunc = ease;
			tw.callback = callbackFunc;
			tw.time = 0;
			tw.percnt = 0;

			activeTweens.push_back(tw);
			return tw;
		}

		static void updateTween(Tween& t, float elapsed)
		{
			t.time += elapsed;
			t.percnt = t.time / t.duration;
			double start = t.start;
			double end = t.end;
			double value = t.easeFunc(t.percnt);
			switch (t.type)
			{
			case tt_Alpha:
				t.obj->alpha = start + ((end - start) * value);
				break;
			case tt_X:
				t.obj->setX(start + ((end - start) * value));
				break;
			case tt_Y:
				t.obj->setY(start + ((end - start) * value));
				break;
			}
			if (t.percnt >= 1)
			{
				// finished
				std::cout << "callback" << std::endl;
				if (t.callback != nullptr)
					t.callback();
				activeTweens.erase(std::remove(activeTweens.begin(), activeTweens.end(), t), activeTweens.end());
			}
		}
	};
}