#include "SongGatherer.h"

void ThreadTask(Average4k::Chart::Collection::SongGatherer* _this, std::vector<Average4k::Chart::Pack>&foundPacks, int i)
{
	Average4k::Chart::Pack p = foundPacks[i];
	std::vector<std::string> charts = _this->FindCharts(p.path);
	for (int c = 0; c < charts.size(); c++)
	{
		std::string cPath = charts[c];
	}
	_this->packs.push_back(p);
	_this->done++;
}

void Average4k::Chart::Collection::SongGatherer::FindPacks(std::string directory)
{

	packs.clear();

	total = 0;
	done = 0;

	//static std::mutex l;

	// itterate through the directory

	std::vector<Pack> foundPacks{};
	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		std::string path = entry.path().string();
		if (entry.is_directory())
		{
			Pack p = Pack(path);
			if (p.good)
				foundPacks.push_back(p);
		}
	}

	total = foundPacks.size() - 1;

	boost::asio::thread_pool pool(8);

	for (int i = 0; i < foundPacks.size(); i++)
	{
		boost::asio::post(pool, boost::bind(ThreadTask, this, foundPacks, i));
	}

}
