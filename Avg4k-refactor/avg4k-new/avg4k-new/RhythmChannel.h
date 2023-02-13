#pragma once
#include "BASS.h"
#include "includes.h"
#include "TimeUtils.h"
namespace Average4k::Audio
{
	struct TimeSegment
	{
		int id = -1;
		double bpm;
		float startTime;
		float length;
		double startBeat;
		double endBeat;
	};

	class RhythmChannel : public AvgEngine::Audio::Channel
	{
	public:
		std::vector<TimeSegment> timeSegments{};
		TimeSegment* lastSegment = NULL;
		RhythmChannel(unsigned long _id) : Channel(_id)
		{
			// empty
		}

		TimeSegment* FindSegment(float time)
		{
			for(TimeSegment& s : timeSegments)
			{
				if (s.startTime <= time && s.startTime + s.length > time)
					return &s;
			}
			return {};
		}

		TimeSegment* FindSegment(double beat)
		{
			for (TimeSegment& s : timeSegments)
			{
				if (s.startBeat <= beat && s.endBeat > beat)
					return &s;
			}
			return {};
		}

		void SetSegments(std::vector<TimeSegment> segs)
		{
			timeSegments = segs;
		}

		double GetBeat()
		{
			if (timeSegments.empty())
				return 0;

			const float pos = GetPos();

			if (lastSegment != NULL)
			{
				if (lastSegment->startTime <= pos && lastSegment->startTime + lastSegment->length < pos)
					goto findSeg;
				return Utils::TimeUtils::ConvertTimeToBeat(lastSegment->bpm, pos, lastSegment->startBeat, lastSegment->startTime);
			}

		findSeg:
			lastSegment = FindSegment(pos);
			if (lastSegment->id == -1)
			{
				AvgEngine::Logging::writeLog("[RhythmChannel] [Error] Failure to get the bpm segment (@ " + std::to_string(static_cast<int>(pos)) + "ms)!");
				return 0;
			}

			return Utils::TimeUtils::ConvertTimeToBeat(lastSegment->bpm, pos, lastSegment->startBeat, lastSegment->startTime);
		}
	};


	/**
	 * \brief A version of BASS.h that is made to create RhythmChannel ptrs instead of Channel ptrs.
	 */
	class RhythmBASSHelper
	{
	public:
		/**
		 * \brief Remove and free the rhythm channel
		 * \param c Channel to be removed
		 */
		static void RemoveChannel(const RhythmChannel* c)
		{
			AvgEngine::External::BASS::Channels.erase(std::ranges::remove_if(AvgEngine::External::BASS::Channels,
				[&](const AvgEngine::Audio::Channel* x) { return x == c; }).begin(), AvgEngine::External::BASS::Channels.end());
		}

		/**
		 * \brief Remove and free the rhythm channel
		 * \param name The name of the channel
		 */
		static void RemoveChannel(std::string name)
		{
			AvgEngine::External::BASS::Channels.erase(std::ranges::remove_if(AvgEngine::External::BASS::Channels,
				[&](const AvgEngine::Audio::Channel* x) { return x->name == name; }).begin(), AvgEngine::External::BASS::Channels.end());
		}

		/**
		 * \brief Remove and free the rhythm channel
		 * \param id The id of the channel
		 */
		static void RemoveChannel(unsigned long id)
		{
			AvgEngine::External::BASS::Channels.erase(std::ranges::remove_if(AvgEngine::External::BASS::Channels,
				[&](const AvgEngine::Audio::Channel* x) { return x->id == id; }).begin(), AvgEngine::External::BASS::Channels.end());
		}

		/**
		 * \brief Get a rhythm channel
		 * \param name The name of the channel
		 */
		static RhythmChannel* GetChannel(const std::string name)
		{
			for (AvgEngine::Audio::Channel* c : AvgEngine::External::BASS::Channels)
				if (c->name == name)
					return static_cast<RhythmChannel*>(c);
			return NULL;
		}


		/**
		 * \brief Get a rhythm channel
		 * \param id The id of the channel
		 */
		static RhythmChannel* GetChannel(const unsigned long id)
		{
			for (AvgEngine::Audio::Channel* c : AvgEngine::External::BASS::Channels)
				if (c->id == id)
					return static_cast<RhythmChannel*>(c);
			return NULL;
		}

		/**
		 * \brief Create a Rhythm BASS Audio Channel
		 * \param name The name of the channel
		 * \param path The file path of the audio
		 * \param autoFree If it should free itself once it is done playing
		 * \return The created channel
		 */

		static RhythmChannel* CreateChannel(const std::string name, const std::string path, bool autoFree = true)
		{
			auto flags = BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT;

			if (autoFree)
				flags |= BASS_STREAM_AUTOFREE;

			std::ostringstream buf;
			std::ifstream inputFile(path, std::ios::binary | std::ios::in);
			buf << inputFile.rdbuf();
			inputFile.close();

			char* buff = static_cast<char*>(std::malloc(buf.str().size()));

			memcpy(buff, buf.str().data(), buf.str().size());

			const auto val = BASS_StreamCreateFile(true, buff, 0, buf.str().size(), flags);

			if (val == 0) {
				if (BASS_ErrorGetCode() != 0) {
					AvgEngine::Logging::writeLog("[BASS] [Error] Error " + std::to_string(BASS_ErrorGetCode()));
				}
				std::free(buff);
				return new RhythmChannel(-1);
			}

			RhythmChannel* c = new RhythmChannel(val);
			c->data = buff;
			c->name = name;

			c->path = path;
			c->SetVolume(0.2);

			const QWORD word = BASS_ChannelGetLength(val, BASS_POS_BYTE);
			c->length = BASS_ChannelBytes2Seconds(val, word) * 1000;

			AvgEngine::External::BASS::Channels.push_back(c);

			return c;
		}
	};
}