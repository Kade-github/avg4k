/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "AsyncChartLoader.h"
#include <AvgEngine/External/Image/stbi.h>
#include <AvgEngine/Utils/StringTools.h>
#include "Providers/StepFile.h"


Average4k::Data::ChartFile Average4k::Data::AsyncChartLoader::chart = {};
std::unordered_map<int, Average4k::Data::asyncImage> Average4k::Data::AsyncChartLoader::textures = {};
int Average4k::Data::AsyncChartLoader::currentId = 0;
std::mutex Average4k::Data::AsyncChartLoader::m_lock;

BS::thread_pool Average4k::Data::AsyncChartLoader::pool = BS::thread_pool();
BS::thread_pool Average4k::Data::AsyncChartLoader::chartPool = BS::thread_pool();

int Average4k::Data::AsyncChartLoader::AsyncLoadTexture(std::wstring path)
{
	int myId = currentId;
	currentId++;
	pool.detach_task([path, myId]() {
		std::string converted_str = AvgEngine::Utils::StringTools::Ws2s(path);

		int w, h;

		unsigned char* data = AvgEngine::External::stbi_h::stbi_load_file_data(converted_str, &w, &h);

		asyncImage a;
		a.data = data;
		a.width = w;
		a.height = h;
		a.path = path;

		{
			std::lock_guard<std::mutex> lock(m_lock);
			textures[myId] = a;
		}
	});
	return myId;
}
void Average4k::Data::AsyncChartLoader::AsyncLoadChart(std::wstring path)
{
	chartPool.detach_task([path]() {
		if (path.ends_with(L".sm") || path.ends_with(L".ssc"))
		{
			Chart::Providers::StepFile stepfile = Chart::Providers::StepFile(path);

			chart = stepfile;
		}
	});
}

void Average4k::Data::AsyncChartLoader::ClearAll()
{
	pool.purge();
	if (pool.get_tasks_queued() + pool.get_tasks_running() > 0)
	{
		pool.wait();
	}
	chartPool.purge();
	if (chartPool.get_tasks_queued() + chartPool.get_tasks_running() > 0)
	{
		chartPool.wait();
	}
	textures.clear();
	chart = ChartFile();


}