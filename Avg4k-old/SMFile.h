#pragma once
#include <sstream>
#include <string>
#include <fstream>
#include "Chart.h"

class SMFile
{
	public:
		bool dontUse = false;
		chartMeta meta;
		SMFile(std::string path, std::string folder, bool doReplace);


		static void SaveSM(chartMeta meta, std::string outFile, float offset = 0);
};

