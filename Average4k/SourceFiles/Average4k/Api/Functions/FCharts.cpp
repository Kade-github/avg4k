/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FCharts.h"
#include "../../Data/Chart/ChartFinder.h"
#include "../../Data/Chart/AsyncChartLoader.h"

#include "../../Steam/UGCHandler.h"
#include "../../Helpers/StringTools.h"

Average4k::Api::AvgLuaFile* Average4k::Api::Functions::FCharts::Lua = nullptr;

void Average4k::Api::Functions::FCharts::ScanCharts()
{
	Lua->getState().collect_garbage();

	Average4k::Data::ChartFinder::FindCharts("Charts");

	Steam::UGCHandler::Instance->PopulateSubscribedItems();

	if (sizeof(Steam::UGCHandler::Instance->subscribedItems) != 0)
	{
		std::jthread th([&]() {
			while (Steam::UGCHandler::Instance->findingSubscribedItems)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			Data::ChartFinder::FindCharts(Steam::UGCHandler::Instance->subscribedPacks);
			});
		th.detach();
	}
	else
	{
		AvgEngine::Logging::writeLog("[Average4k] [Warning] No subscribed packs found.");
	}
}

sol::table Average4k::Api::Functions::FCharts::GetPacks()
{
	Lua->getState().collect_garbage();
	sol::table t = Lua->getState().create_table();

	for (int i = 0; i < Average4k::Data::ChartFinder::Packs.size(); i++)
	{
		Average4k::Data::Pack& p = Average4k::Data::ChartFinder::Packs[i];

		sol::table pack = Lua->getState().create_table();

		pack["name"] = p.name;
		pack["path"] = p.path;
		pack["banner"] = p.banner;

		t[i + 1] = pack;
	}

	return t;
}

sol::table Average4k::Api::Functions::FCharts::GetCharts(std::wstring pack)
{
	Lua->getState().collect_garbage();
	for (int i = 0; i < Average4k::Data::ChartFinder::Packs.size(); i++)
	{
		Average4k::Data::Pack& p = Average4k::Data::ChartFinder::Packs[i];

		if (p.name == pack)
		{
			sol::table charts = Lua->getState().create_table();

			std::wstring_convert<std::codecvt_utf8<char8_t>, char8_t> convert;

			for (int j = 0; j < p.charts.size(); j++)
			{
				Average4k::Data::Chart::Providers::StepFile c = p.charts[j];

				sol::table chart = Lua->getState().create_table();



				chart["title"] = convert.from_bytes(Average4k::Helpers::StringTools::Ws2s(c.metadata.title));
				chart["titleTranslit"] = c.metadata.titleTranslit;
				chart["subtitle"] = convert.from_bytes(Average4k::Helpers::StringTools::Ws2s(c.metadata.subtitle));
				chart["subtitleTranslit"] = c.metadata.subtitleTranslit;
				chart["artist"] = convert.from_bytes(Average4k::Helpers::StringTools::Ws2s(c.metadata.artist));
				chart["artistTranslit"] = c.metadata.artistTranslit;
				chart["genre"] = c.metadata.genre;
				chart["credit"] = convert.from_bytes(Average4k::Helpers::StringTools::Ws2s(c.metadata.credit));
				chart["banner"] = c.metadata.banner;
				chart["background"] = c.metadata.background;
				chart["file"] = c.metadata.file;
				chart["offset"] = c.metadata.offset;
				chart["previewStart"] = c.metadata.previewStart;

				chart["type"] = c.metadata.type;
				chart["path"] = c.path;

				chart["workshop"] = c.metadata.workshop;
				// get folder from path
				chart["folder"] = c.path.substr(0, c.path.find_last_of(L"\\"));

				charts[j + 1] = chart;
			}

			return charts;
		}
	}

	return Lua->getState().create_table();
}

void Average4k::Api::Functions::FCharts::LoadChart(std::wstring path)
{
	Average4k::Data::AsyncChartLoader::AsyncLoadChart(path);
}

int Average4k::Api::Functions::FCharts::LoadAsyncTexture(std::wstring path)
{
	return Average4k::Data::AsyncChartLoader::AsyncLoadTexture(path);
}

void Average4k::Api::Functions::FCharts::LoadAsyncAudio(std::string name, std::string path)
{
	Average4k::Data::AsyncChartLoader::AsyncLoadAudio(path, name);
}

void Average4k::Api::Functions::FCharts::ClearAudio()
{
	for (AvgEngine::Audio::Channel* c : AvgEngine::External::BASS::Channels)
	{
		if (c == NULL || c->id == -1)
			continue;
		if (c->isPlaying)
			c->Stop();
		c->Free();
		delete c;
	}

	AvgEngine::External::BASS::Channels.clear();
}

Average4k::Api::Stubs::LuaSong Average4k::Api::Functions::FCharts::GetAsyncAudio()
{
	AvgEngine::Audio::Channel* audio = Average4k::Data::AsyncChartLoader::CheckAudio();

	if (audio == nullptr)
		return Average4k::Api::Stubs::LuaSong();

	Lua->getState().collect_garbage();

	Average4k::Api::Stubs::LuaSong song = Average4k::Api::Stubs::LuaSong(audio);

	return song;
}

Average4k::Api::Stubs::LuaSprite Average4k::Api::Functions::FCharts::GetAsyncTexture(int id)
{
	Average4k::Data::asyncImage img = Average4k::Data::AsyncChartLoader::checkTexture(id);

	if (img.data == nullptr)
		return Average4k::Api::Stubs::LuaSprite();

	Lua->getState().collect_garbage();

	Average4k::Api::Stubs::LuaSprite spr = Average4k::Api::Stubs::LuaSprite(-100, -100, img.data, img.width, img.height);

	return spr;
}

sol::table Average4k::Api::Functions::FCharts::CheckChart()
{
	Average4k::Data::ChartFile chart = Average4k::Data::AsyncChartLoader::CheckChart();

	Lua->getState().collect_garbage();

	sol::table t = Lua->getState().create_table();

	if (chart.metadata.title == L"")
	{
		t["title"] = "";
		return t;
	}

	t["title"] = chart.metadata.title;
	t["titleTranslit"] = chart.metadata.titleTranslit;
	t["subtitle"] = chart.metadata.subtitle;
	t["subtitleTranslit"] = chart.metadata.subtitleTranslit;
	t["artist"] = chart.metadata.artist;
	t["artistTranslit"] = chart.metadata.artistTranslit;
	t["genre"] = chart.metadata.genre;
	t["credit"] = chart.metadata.credit;
	t["banner"] = chart.metadata.banner;
	t["background"] = chart.metadata.background;
	t["file"] = chart.metadata.file;
	t["offset"] = chart.metadata.offset;
	t["previewStart"] = chart.metadata.previewStart;

	t["type"] = chart.metadata.type;

	t["workshop"] = chart.metadata.workshop;
	t["path"] = chart.path;

	t["timingPoints"] = Lua->getState().create_table();

	for (int i = 0; i < chart.timingPoints.size(); i++)
	{
		Average4k::Data::Chart::TimingPoint& tp = chart.timingPoints[i];

		sol::table timingPoint = Lua->getState().create_table();

		timingPoint["startBeat"] = tp.startBeat;
		timingPoint["endBeat"] = tp.endBeat;
		timingPoint["bpm"] = tp.bpm;

		timingPoint["startTimestamp"] = tp.startTimestamp;
		timingPoint["endTimestamp"] = tp.endTimestamp;
		timingPoint["length"] = tp.length;

		t["timingPoints"][i + 1] = timingPoint;
	}

	t["stopPoints"] = Lua->getState().create_table();

	for (int i = 0; i < chart.stopPoints.size(); i++)
	{
		Average4k::Data::Chart::StopPoint& sp = chart.stopPoints[i];

		sol::table stopPoint = Lua->getState().create_table();

		stopPoint["startBeat"] = sp.startBeat;
		stopPoint["startTimestamp"] = sp.startTimestamp;
		stopPoint["length"] = sp.length;


		t["stopPoints"][i + 1] = stopPoint;
	}

	t["difficulties"] = Lua->getState().create_table();

	for (int i = 0; i < chart.difficulties.size(); i++)
	{
		Average4k::Data::Chart::Difficulty& d = chart.difficulties[i];

		sol::table difficulty = Lua->getState().create_table();

		difficulty["name"] = d.name;
		difficulty["charter"] = d.charter;
		difficulty["rating"] = d.rating;

		difficulty["notes"] = Lua->getState().create_table();

		for (int j = 0; j < d.notes.size(); j++)
		{
			Average4k::Data::Chart::Note& n = d.notes[j];

			sol::table note = Lua->getState().create_table();

			note["beat"] = n.beat;
			note["lane"] = n.lane;
			note["type"] = n.type;

			difficulty["notes"][j + 1] = note;
		}


		t["difficulties"][i + 1] = difficulty;
	}

	return t;
}

void Average4k::Api::Functions::FCharts::ClearAsync()
{
	Average4k::Data::AsyncChartLoader::ClearAll();
}
