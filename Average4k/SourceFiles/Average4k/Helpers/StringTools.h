/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_STRINGTOOLS_H
#define AVG4K_STRINGTOOLS_H

#pragma once

#include <string>

namespace Average4k::Helpers
{
	class StringTools
	{
	public:
		static std::string ToTheDecimial(double value, int decimalPlaces)
		{
			std::string str = std::to_string(value);
			int decimalIndex = str.find('.');

			if (decimalIndex == std::string::npos)
				return str;

			return str.substr(0, decimalIndex + decimalPlaces + 1);
		}
	};
}

#endif