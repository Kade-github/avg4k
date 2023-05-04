#pragma once
#include "includes.h"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <boost/bind.hpp>

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

		std::vector< boost::asio::thread_pool> pools;

		void FindPacks(std::string directory, int threads);
		ChartFile* FindSong(std::string path, Pack& pack);

		std::map<std::string, std::string> FindCharts(std::string dir)
		{
			std::map<std::string, std::string> charts = {};
			// itterate through the directory
			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				std::string path = entry.path().string();
				if (entry.is_directory())
				{
					// itterate through all the files in this directory, find a chart file.
					for (const auto& file : std::filesystem::directory_iterator(path))
					{
						std::string ext = file.path().extension().string();

						if (file.is_regular_file() && (ext == ".sm" || ext == ".ssc"))
						{
							std::string p = file.path().string();
							charts[path] = p;
							break;
						}
					}
				}
				
			}
			return charts;
		}
	};
}