/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef TWEENMANAGER_H
#define TWEENMANAGER_H

#pragma once
#include <AvgEngine/Utils/Easing.h>
#include <AvgEngine/Render/Display.h>
#include <functional>
#include <algorithm>

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

		void Clear()
		{
			Tweens.clear();
		}

		void DoTween(Tween tw)
		{
			double curTime = std::abs(glfwGetTime() - tw.startTime);
			double t = std::clamp(curTime / tw.length, 0.0, 1.0);
			double rT = tw.ease(t);

			tw.toModify->x = std::lerp(tw.start.x, tw.end.x, rT);
			tw.toModify->y = std::lerp(tw.start.y, tw.end.y, rT);
			tw.toModify->a = std::lerp(tw.start.a, tw.end.a, rT);
			tw.toModify->r = std::lerp(tw.start.r, tw.end.r, rT);
			tw.toModify->g = std::lerp(tw.start.g, tw.end.g, rT);
			tw.toModify->b = std::lerp(tw.start.b, tw.end.b, rT);
			tw.toModify->scale = std::lerp(tw.start.scale, tw.end.scale, rT);
			if (t >= 1)
			{
				if (tw.func)
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

#endif // !TWEENMANAGER_H