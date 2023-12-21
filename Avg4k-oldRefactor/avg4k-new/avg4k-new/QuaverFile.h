#pragma once
#include "ChartFile.h"

namespace Average4k::Chart
{
	class QuaverFile : public ChartFile {
		QuaverFile(std::string _path) : ChartFile(_path)
		{
			chartMetadata.Chart_Type = 1;
		}

		void Parse() override;
	};
}