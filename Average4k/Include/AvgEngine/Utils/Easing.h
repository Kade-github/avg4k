/*
	This file is not under the license
*/

#ifndef EASING_H
#define EASING_H

#pragma once 

#include <iostream>
namespace AvgEngine::Easing
{
	class Easing {
	public:
		enum easing_functions
		{
			EaseLinear,
			EaseInSine,
			EaseOutSine,
			EaseInQuad,
			EaseOutQuad,
			EaseInCubic,
			EaseOutCubic,
			EaseInQuart,
			EaseOutQuart,
			EaseInQuint,
			EaseOutQuint,
			EaseInExpo,
			EaseOutExpo,
			EaseInCirc,
			EaseOutCirc,
			EaseInBack,
			EaseOutBack,
			EaseInElastic,
			EaseOutElastic,
			EaseInBounce,
			EaseOutBounce,
			Nothing = -1
		};

		typedef double(*easingFunction)(double);

		static easingFunction getEasingFunction(easing_functions function);
		static easingFunction getEasingFunction(std::string function);
	};
}

#endif // !EASING_H