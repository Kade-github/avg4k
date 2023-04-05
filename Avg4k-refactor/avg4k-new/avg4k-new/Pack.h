#pragma once
#include "ChartFile.h"
#include "includes.h"
#include "Texture.h"
#include "ConfigReader.h"
namespace Average4k::Chart
{
	class Pack {
	public:
		std::string path = "";

		std::string name = "";
		std::string bannerPath = "";
		bool showName = false;

		std::vector<ChartFile*> files = {};

		bool good = false;


		Pack(std::string _path)
		{
			path = _path;

			Average4k::External::ConfigReader r = Average4k::External::ConfigReader(path + "/pack.meta");
			if (!r.good)
			{
				AvgEngine::Logging::writeLog("[Pack] [Error] Couldn't find " + path + "/pack.meta!");
				return;
			}
			if (!r.Contains("packName"))
				AvgEngine::Logging::writeLog("[Pack] [Warning] " + path + " does not contain a packName config value.");
			else
				name = r.Value("packName");
			if (!r.Contains("banner"))
				AvgEngine::Logging::writeLog("[Pack] [Warning] " + path + " does not contain a banner config value.");
			else
			{
				bannerPath = r.Value("banner");
			}

			if (!r.Contains("showName"))
				AvgEngine::Logging::writeLog("[Pack] [Warning] " + path + " does not contain a showName config value.");
			else
				showName = r.Bool("showName");

			good = true;
		}

	};
}