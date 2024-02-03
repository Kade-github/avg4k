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
		// Storage variables for measures
		int currentMeasure = 0;
		std::vector<std::string> measure;
		std::ifstream stream;

		// Storage for difficulty
		Difficulty workingDiff;
		bool skipDiff = false;

		void ParseMetadata(bool only);
		void ParseBPMS(std::string line);
		void ParseStops(std::string line);
		void ParseNotes(std::string line, int& lineNumber);
	public:
		void Parse(std::string path, bool metadataOnly) override;
	};
}

#endif