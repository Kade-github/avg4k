/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include <filesystem>

#include "ChartFinder.h"
#include <mutex>

using namespace Average4k::Data;

std::mutex ChartFinder::m_lock = {};

std::vector<Pack> ChartFinder::Packs = {};

BS::thread_pool ChartFinder::pack_pool = BS::thread_pool(std::thread::hardware_concurrency() * 3.334f); // MAGIC NUMBER

bool ChartFinder::startedSearching = false;

void ChartFinder::FindCharts(const std::vector<std::string> paths)
{
	std::vector<std::string> p = paths;
	startedSearching = true;

	for (const auto& path : p)
	{
		if (path.empty())
			continue;

		std::wstring path = path;

		std::string s_path = AvgEngine::Utils::StringTools::Ws2s(path);
		std::wstring name = path.substr(path.find_last_of(L"\\") + 1);

		{
			std::lock_guard<std::mutex> lock(m_lock);
			// check if pack already exists

			if (std::find_if(Packs.begin(), Packs.end(), [name](const Pack& p) { return p.name == name; }) != Packs.end())
				continue;
		}

		pack_pool.detach_task([&, path, s_path, name]()
			{
				Pack p;

				// get directory name from path


				p.name = name;
				p.path = path;

				if (p.name.empty())
					return;

				// if the pack is already in the list, skip
				{
					std::lock_guard<std::mutex> lock(m_lock);
					if (std::find_if(Packs.begin(), Packs.end(), [p](const Pack& c) { return c.name == p.name; }) != Packs.end())
						return;
				}

				std::vector<std::wstring> paths;
				std::vector<std::wstring> folders;
				// get banner
				for (const auto& e : std::filesystem::directory_iterator(path))
				{
					std::wstring pth = e.path().wstring();
					if ((pth.ends_with(L".png") || pth.ends_with(L".jpg") || pth.ends_with(L".jpeg")) && p.banner.empty())
					{
						// get banner name from path

						std::wstring bnnrFile = pth.substr(pth.find_last_of(L"\\") + 1);

						p.banner = bnnrFile;
						continue;
					}
				}

				for (const auto& e : std::filesystem::recursive_directory_iterator(path))
				{
					std::wstring pth = e.path().wstring();

					std::wstring fldr = pth.substr(0, pth.find_last_of(L"\\"));
					if (std::find(folders.begin(), folders.end(), fldr) != folders.end())
						continue;

					if (pth.ends_with(L".sm") || pth.ends_with(L".ssc"))
					{
						paths.push_back(pth);
						folders.push_back(fldr);
					}
				}

				BS::thread_pool chart_pool(std::thread::hardware_concurrency());


				std::mutex m;
				AvgEngine::Logging::writeLog("[ChartFinder] Scanning for charts in " + s_path + " with " + std::to_string(chart_pool.get_thread_count()) + " threads.");
				chart_pool.detach_loop<unsigned int>(0, paths.size(),
					[&](const unsigned int i)
					{
						std::wstring path = paths[i];

						if (path.ends_with(L".sm") || path.ends_with(L".ssc"))
						{
							Chart::Providers::StepFile stepfile = Chart::Providers::StepFile(path, true); // only metadata

							if (stepfile.isValid)
							{
								std::lock_guard<std::mutex> lock(m);
								// if the name, artist are the same probably skip
								if (std::find_if(p.charts.begin(), p.charts.end(), [stepfile](const Chart::Providers::StepFile& c) { return c.metadata.title == stepfile.metadata.title && c.metadata.artist == stepfile.metadata.artist; }) != p.charts.end())
									return;
								p.charts.push_back(stepfile);
							}
						}
					});
				chart_pool.wait();
				{
					std::lock_guard<std::mutex> lock(m_lock);
					if (p.charts.size() == 0)
						return;
					Packs.push_back(p);
				}
			});

	}
	startedSearching = false;
}

void ChartFinder::FindCharts(const std::string _path)
{
	if (pack_pool.get_tasks_running() != 0)
	{
		pack_pool.wait();
	}
	Packs.clear();
	AvgEngine::Logging::writeLog("[ChartFinder] Scanning for packs in " + _path + " with " + std::to_string(pack_pool.get_thread_count()) + " threads.");

	std::string p = _path;
	startedSearching = true;

	std::thread t([p]() {
		for (const auto& entry : std::filesystem::directory_iterator(p))
		{
			if (entry.path().empty())
				continue;

			std::wstring path = entry.path().wstring();

			if (entry.is_directory())
			{
				std::string s_path = AvgEngine::Utils::StringTools::Ws2s(path);
				std::wstring name = path.substr(path.find_last_of(L"\\") + 1);

				{
					std::lock_guard<std::mutex> lock(m_lock);
					// check if pack already exists

					if (std::find_if(Packs.begin(), Packs.end(), [name](const Pack& p) { return p.name == name; }) != Packs.end())
						continue;
				}
				pack_pool.detach_task([&, path, s_path, name]()
					{
						Pack p;

						// get directory name from path


						p.name = name;
						p.path = path;

						if (p.name.empty())
							return;

						// if the pack is already in the list, skip
						{
							std::lock_guard<std::mutex> lock(m_lock);
							if (std::find_if(Packs.begin(), Packs.end(), [p](const Pack& c) { return c.name == p.name; }) != Packs.end())
								return;
						}

						std::vector<std::wstring> paths;
						std::vector<std::wstring> folders;
						// get banner
						for (const auto& e : std::filesystem::directory_iterator(path))
						{
							std::wstring pth = e.path().wstring();
							if ((pth.ends_with(L".png") || pth.ends_with(L".jpg") || pth.ends_with(L".jpeg")) && p.banner.empty())
							{
								// get banner name from path

								std::wstring bnnrFile = pth.substr(pth.find_last_of(L"\\") + 1);

								p.banner = bnnrFile;
								continue;
							}
						}

						for (const auto& e : std::filesystem::recursive_directory_iterator(path))
						{
							std::wstring pth = e.path().wstring();

							std::wstring fldr = pth.substr(0, pth.find_last_of(L"\\"));
							if (std::find(folders.begin(), folders.end(), fldr) != folders.end())
								continue;

							if (pth.ends_with(L".sm") || pth.ends_with(L".ssc"))
							{
								paths.push_back(pth);
								folders.push_back(fldr);
							}
						}

						BS::thread_pool chart_pool(std::thread::hardware_concurrency());


						std::mutex m;
						AvgEngine::Logging::writeLog("[ChartFinder] Scanning for charts in " + s_path + " with " + std::to_string(chart_pool.get_thread_count()) + " threads.");
						chart_pool.detach_loop<unsigned int>(0, paths.size(),
							[&](const unsigned int i)
							{
								std::wstring path = paths[i];

								if (path.ends_with(L".sm") || path.ends_with(L".ssc"))
								{
									Chart::Providers::StepFile stepfile = Chart::Providers::StepFile(path, true); // only metadata

									if (stepfile.isValid)
									{
										std::lock_guard<std::mutex> lock(m);
										// if the name, artist are the same probably skip
										if (std::find_if(p.charts.begin(), p.charts.end(), [stepfile](const Chart::Providers::StepFile& c) { return c.metadata.title == stepfile.metadata.title && c.metadata.artist == stepfile.metadata.artist; }) != p.charts.end())
											return;
										p.charts.push_back(stepfile);
									}
								}
							});
						chart_pool.wait();
						{
							std::lock_guard<std::mutex> lock(m_lock);
							if (p.charts.size() == 0)
								return;
							Packs.push_back(p);
						}
					});
			}
		}
		startedSearching = false;
		});
	t.detach();
}
