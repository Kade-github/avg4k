#pragma once
#include "includes.h"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

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
		int total = 0;
		int done = 0;
		std::vector<Pack> packs = {};

		std::vector<std::thread> threads{};

		void FindPacks(std::string directory)
		{
			// itterate through the directory

			packs.clear();

			total = 0;
			done = 0;

			static std::mutex l;

			boost::asio::thread_pool pool(8);

			
	
		}

		std::vector<std::string> FindCharts(std::string dir)
		{
			std::vector<std::string> charts = {};
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
							std::string p = file.path().string();
							charts.push_back(p);
						}
					}
				}
				
			}
			return charts;
		}
	};
}