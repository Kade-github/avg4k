#pragma once
#include <sstream>
#include <string>
#include <fstream>
#include "Chart.h"

class SMFile
{
	public:
		chartMeta meta;
		SMFile(std::string path, std::string folder, bool doReplace);
};

