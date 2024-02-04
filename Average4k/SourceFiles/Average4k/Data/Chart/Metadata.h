/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_METADATA_H
#define _AVG4K_METADATA_H

#include <iostream>

namespace Averag4k::Data 
{
	enum ChartType {
		Step = 0,
		Quaver = 1,
		Mania = 2,
		A4k = 3,
		Unknown = -1
	};
}

namespace Average4k::Data::Chart
{

	class Metadata
	{
	public:
		std::wstring artist = L"";
		std::wstring title = L"";
		std::wstring subtitle = L"";
		std::wstring genre = L"";
		std::wstring credit = L"";
		std::wstring banner = L"";
		std::wstring background = L"";
		std::wstring file = L"";

		float offset = 0;
		float previewStart = 0;

		Averag4k::Data::ChartType type = Averag4k::Data::ChartType::Unknown;

		bool workshop = false;
	};
}

#endif