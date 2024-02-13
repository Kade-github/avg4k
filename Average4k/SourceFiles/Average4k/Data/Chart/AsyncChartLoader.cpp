/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "AsyncChartLoader.h"
#include <AvgEngine/External/Image/stbi.h>
#include <AvgEngine/Utils/StringTools.h>
#include "Providers/StepFile.h"

#include "../../A4kGame.h"


Average4k::Data::ChartFile Average4k::Data::AsyncChartLoader::chart = {};
std::unordered_map<int, Average4k::Data::asyncImage> Average4k::Data::AsyncChartLoader::textures = {};
int Average4k::Data::AsyncChartLoader::currentId = 0;
std::mutex Average4k::Data::AsyncChartLoader::m_lock;
std::mutex Average4k::Data::AsyncChartLoader::s_lock;
AvgEngine::Audio::Channel* Average4k::Data::AsyncChartLoader::channel = nullptr;

std::string Average4k::Data::AsyncChartLoader::latestAudioPath = "";


BS::thread_pool Average4k::Data::AsyncChartLoader::pool = BS::thread_pool(); // default thread pool for bgs/banners
BS::thread_pool Average4k::Data::AsyncChartLoader::chartPool = BS::thread_pool(1); // only one thread for chart
BS::thread_pool Average4k::Data::AsyncChartLoader::audioPool = BS::thread_pool(1); // only one thread for audio

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

void Average4k::Data::AsyncChartLoader::AsyncLoadAudio(std::string path, std::string name)
{
	std::string p = path;
	latestAudioPath = p;
	audioPool.purge();
	audioPool.detach_task([p, name]() {
		LoadAudio(p, name);
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

void Average4k::Data::AsyncChartLoader::LoadAudio(std::string path, std::string name)
{
	std::lock_guard<std::mutex> lock(s_lock);
	channel = AvgEngine::External::BASS::CreateChannel(name, path, false);

	Data::SaveData a = A4kGame::gameInstance->saveData;

	for (auto c : AvgEngine::External::BASS::Channels)
	{
		if (c->name.contains("sfx_"))
		{
			c->SetVolume(a.audioData.sfxVolume);
		}
		else
		{
			c->SetVolume(a.audioData.volume);
		}
	}
}
