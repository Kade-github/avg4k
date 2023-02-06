#pragma once
#include "includes.h"
#include "Easing.h"
#include "Display.h"

namespace AvgEngine
{
	struct Tween
	{
		int id = 0;
		std::function<void()> func = NULL;
		double startTime = 0;
		double length = 0;

		Render::Rect* toModify{};
		Render::Rect start;
		Render::Rect end;

		Easing::Easing::easingFunction ease = NULL;

		bool operator==(const Tween& other) {
			return id == other.id;
		}
	};

	class TweenManager
	{
	public:
		int lastId = 0;
		std::vector<Tween> Tweens{};

		/**
		 * \brief Create a tween which does a Linear Interpolation Curve between two rectangles, using a custom easing function
		 * \param toModify A reference to the rectangle to modify
		 * \param end What the rectangle should be when it ends
		 * \param length The length in seconds of the tween
		 * \param ease The custom easing function
		 * \param func A function to be called when the tween ends (can be null)
		 */
		void CreateTween(Render::Rect* toModify, Render::Rect end, double length, Easing::Easing::easingFunction ease, std::function<void()> func)
		{
			if (toModify == NULL)
			{
				Logging::writeLog("[Error] Failed to create a tween; toModify was null.");
				return;
			}
			Tween t;
			t.toModify = toModify;
			t.start = *toModify;
			t.end = end;
			t.length = length;
			t.startTime = glfwGetTime();
			t.ease = ease;
			t.func = func;
			t.id = lastId;
			lastId++;
			Tweens.push_back(t);
		}

		void DoTween(Tween tw)
		{
			double curTime = std::abs(glfwGetTime() - tw.startTime);
			double t = std::min(curTime / tw.length, 1.0);
			double rT = tw.ease(t);
			Logging::writeLog("Tween value: " + std::to_string(rT));
			tw.toModify->x = std::lerp(tw.start.x, tw.end.x, rT);
			tw.toModify->y = std::lerp(tw.start.y, tw.end.y, rT);
			tw.toModify->a = std::lerp(tw.start.a, tw.end.a, rT);
			tw.toModify->r = std::lerp(tw.start.r, tw.end.r, rT);
			tw.toModify->g = std::lerp(tw.start.g, tw.end.g, rT);
			tw.toModify->b = std::lerp(tw.start.b, tw.end.b, rT);
			if (tw.func && t >= 1)
			{
				tw.func();
				Tweens.erase(std::ranges::remove_if(Tweens,
					[&](const Tween x) { return x == tw; }).begin(), Tweens.end());
			}
		}

		void Update()
		{
			std::vector<Tween> Copy = Tweens;

			for (Tween t : Copy)
				DoTween(t);
		}
	};
}
