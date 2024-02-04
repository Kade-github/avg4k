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
	static BS::thread_pool pool(std::thread::hardware_concurrency() * 3.334f); // MAGIC NUMBER
	static std::vector<std::wstring> paths;
	static std::chrono::steady_clock::time_point start;

	if (pool.get_tasks_running() != 0)
	{
		pool.wait();
	}

	paths.clear();
	AvgEngine::Logging::writeLog("[ChartFinder] Scanning for charts in " + _path + " with " + std::to_string(pool.get_thread_count()) + " threads.");

	// start timestamp
	start = std::chrono::high_resolution_clock::now();

	std::vector<std::wstring> folders = {};

	for (const auto& entry : std::filesystem::recursive_directory_iterator(_path))
	{
		if (entry.path().empty())
			continue;

		std::wstring path = entry.path().wstring();

		// check if we already found one in this folder, if so skip
		if (std::find(folders.begin(), folders.end(), entry.path().parent_path().wstring()) != folders.end())
			continue;

		if (path.ends_with(L".sm") || path.ends_with(L".ssc"))
		{
			folders.push_back(entry.path().parent_path().wstring());
			paths.push_back(entry.path().wstring());
		}
	}

	pool.detach_loop<unsigned int>(0, paths.size(),
	[&](const unsigned int i)
	{
		std::wstring path = paths[i];
		if (path.ends_with(L".sm") || path.ends_with(L".ssc"))
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
