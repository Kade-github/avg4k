#pragma once
#include "includes.h"
#include <boost/thread.hpp>
#include "StepFile.h"
#include "Pack.h"

namespace Average4k::Chart::Collection
{
	/// <summary>
	/// A collection of functions and helpers to mass gather packs/songs from a directory.
	/// </summary>
	class SongGatherer
	{
	public:
		int done = 0;
		bool finished = false;
		std::vector<Pack> packs = {};

		std::vector<std::thread> threads{};

		void FindPacks(std::string directory)
		{
			// itterate through the directory
			finished = false;
			packs.clear();


			done = 0;

			// TODO: multithread the shit out of this (too lazy rn)


			for (const auto& entry : std::filesystem::directory_iterator(directory))
			{
				if (entry.is_directory())
				{
					std::string path = entry.path().string();
					Pack p = Pack(path);
					if (p.good)
					{
						std::vector<ChartFile> charts = FindCharts(path);

						// copy all the charts found onto the pack.
						for (ChartFile c : charts)
							p.files.push_back(c);

						// push the pack onto the list
						packs.push_back(p);
					}
					done++;

				}
			}
			finished = true;

		}

		std::vector<ChartFile> FindCharts(std::string dir)
		{
			std::vector<ChartFile> charts = {};
			// itterate through the directory
			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				std::string path = entry.path().string();
				if (entry.is_directory())
				{
					// itterate through all the files in this directory, find a chart file.
					for (const auto& file : std::filesystem::directory_iterator(path))
					{
						if (file.is_regular_file())
						{
							std::string ext = file.path().extension().string();
							if (ext == ".sm" || ext == ".ssc") // Stepmania/Stepmania 5
							{
								StepFile f = StepFile(file.path().string());
								f.Parse();
								if (f.good)
									charts.push_back(f);
							}
						}
					}
				}
				
			}
			return charts;
		}
	};
}