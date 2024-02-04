/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include <BS_thread_pool.hpp>
#include <BS_thread_pool_utils.hpp>
#include <filesystem>

#include "ChartFinder.h"
#include <mutex>

using namespace Average4k::Data;

std::mutex ChartFinder::m_lock = {};

std::vector<ChartFile> ChartFinder::Charts = {};

void ChartFinder::FindCharts(const std::string _path)
{
	static BS::thread_pool pool; // creates a thread pool with the maximum number of threads available on the system
	static std::vector<std::string> paths;
	static std::chrono::steady_clock::time_point start;

	if (pool.get_tasks_running() != 0)
	{
		pool.wait();
	}

	paths.clear();
	AvgEngine::Logging::writeLog("[ChartFinder] Scanning for charts in " + _path + " with " + std::to_string(pool.get_thread_count()) + " threads.");

	// start timestamp
	start = std::chrono::high_resolution_clock::now();

	for (const auto& entry : std::filesystem::recursive_directory_iterator(_path))
	{
		if (entry.path().empty())
			continue;
		if (entry.path().extension() == ".sm" || entry.path().extension() == ".ssc")
			paths.push_back(entry.path().string());
	}

	pool.detach_loop<unsigned int>(0, paths.size(),
	[&](const unsigned int i)
	{
		std::string path = paths[i];
		if (path.ends_with(".sm") || path.ends_with(".ssc"))
		{
			Chart::Providers::StepFile stepfile = Chart::Providers::StepFile(path, true); // only metadata

			if (stepfile.isValid)
			{
				std::lock_guard<std::mutex> lock(m_lock);
				Charts.push_back(stepfile);
			}
		}
	});

	std::thread([&]()
	{
		pool.wait();
		// end timestamp
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;

		AvgEngine::Logging::writeLog("[ChartFinder] Found " + std::to_string(paths.size()) + " charts in " + std::to_string(elapsed.count()) + " seconds");
	}).detach();

}
