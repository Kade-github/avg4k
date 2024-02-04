/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_STEP_FILE_H
#define _AVG4K_STEP_FILE_H

#include "../ChartFile.h"

namespace Average4k::Data::Chart::Providers
{
	class StepFile : public ChartFile
	{
	public:
		StepFile();

		StepFile(std::wstring path, bool metadataOnly = false)
		{
			Parse(path, metadataOnly);
		}

		// Storage variables for measures
		int currentMeasure = 0;
		std::vector<std::wstring> measure = {};

		// Storage for difficulty
		Difficulty workingDiff;
		bool skipDiff = false;

		void ParseMetadata(bool only, std::wifstream* stream);
		void ParseBPMS(std::wstring line);
		void ParseStops(std::wstring line);
		void ParseNotes(std::wstring line, int& lineNumber);

		void Parse(std::wstring path, bool metadataOnly) override;
	};
}

#endif