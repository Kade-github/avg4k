#include "SongGatherer.h"

std::mutex songGatherer_mutex;

void ThreadTask(Average4k::Chart::Collection::SongGatherer* _this, std::string cPath, Average4k::Chart::Pack& pack)
{
	try
	{
		Average4k::Chart::StepFile f = Average4k::Chart::StepFile(cPath);
		f.Parse();
		songGatherer_mutex.lock();
		if (f.good)
		{
			pack.files.push_back(f);
		}
		_this->done++;
		songGatherer_mutex.unlock();
	}
	catch (const std::exception& e)
	{
		AvgEngine::Logging::writeLog("[SongGatherer] [Error] " + cPath + " failed to load! (" + e.what() + ")");
		_this->done++;
	}
	catch (...)
	{
		AvgEngine::Logging::writeLog("[SongGatherer] [Error] " + cPath + " failed to load! (and honestly I have no idea what the fuck happened)");
		_this->done++;
	}
}

void Average4k::Chart::Collection::SongGatherer::FindPacks(std::string directory, int threads)
{
	pools.clear();
	packs.clear();

	total = 0;
	done = 0;

	//static std::mutex l;

	// itterate through the directory

	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		std::string path = entry.path().string();
		if (entry.is_directory())
		{
			Pack p = Pack(path);
			if (p.good)
			{
				packs.push_back(p);
			}
		}
	}


	boost::asio::thread_pool pool(threads);

	int totalC = 0;

	for (int i = 0; i < packs.size(); i++)
	{
		std::vector<std::string> charts = {};
		Pack& p = packs[i];
		charts = FindCharts(p.path);
		total += charts.size();
		for (int j = 0; j < charts.size(); j++)
		{
			boost::asio::post(pool, boost::bind(ThreadTask, this, charts[j], std::ref(p)));
			totalC++;
		}
	}
	pool.wait();
}
