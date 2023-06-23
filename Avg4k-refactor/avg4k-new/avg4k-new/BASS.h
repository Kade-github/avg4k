#pragma once
#include "includes.h"
#include "Channel.h"

namespace AvgEngine::External
{
	/**
	 * \brief Helper class to create Audio Channels through the BASS Library.
	 */
	class BASS
	{
	public:
		static std::vector<Audio::Channel*> Channels;

		/**
		 * \brief Initialize the BASS Audio Library.
		 */
		static void Initialize()
		{
			BASS_Init(-1, 44100, 0, NULL, NULL);
			BASS_SetConfig(BASS_CONFIG_ASYNCFILE_BUFFER, 10000); //32MB
		}

		/**
		 * \brief Remove and free the channel
		 * \param c Channel to be removed
		 */
		static void RemoveChannel(const Audio::Channel* c)
		{
			Channels.erase(std::ranges::remove_if(Channels,
				[&](const Audio::Channel* x) { return x == c; }).begin(), Channels.end());
		}

		/**
		 * \brief Remove and free the channel
		 * \param name The name of the channel
		 */
		static void RemoveChannel(std::string name)
		{
			Channels.erase(std::ranges::remove_if(Channels,
				[&](const Audio::Channel* x) { return x->name == name; }).begin(), Channels.end());
		}

		/**
		 * \brief Remove and free the channel
		 * \param id The id of the channel
		 */
		static void RemoveChannel(unsigned long id)
		{
			Channels.erase(std::ranges::remove_if(Channels,
				[&](const Audio::Channel* x) { return x->id == id; }).begin(), Channels.end());
		}

		/**
		 * \brief Get the channel
		 * \param name The name of the channel
		 */
		static Audio::Channel* GetChannel(const std::string name)
		{
			for (Audio::Channel* c : Channels)
				if (c->name == name)
					return c;
			return NULL;
		}


		/**
		 * \brief Get the channel
		 * \param id The id of the channel
		 */
		static Audio::Channel* GetChannel(const unsigned long id)
		{
			for (Audio::Channel* c : Channels)
				if (c->id == id)
					return c;
			return NULL;
		}

		/**
		 * \brief Create a BASS Audio Channel
		 * \param name The name of the channel
		 * \param path The file path of the audio
		 * \param autoFree If it should free itself once it is done playing
		 * \return The created channel
		 */

		static Audio::Channel* CreateChannel(const std::string name, const std::string path, bool autoFree = true)
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
					Logging::writeLog("[BASS] [Error] Error " + std::to_string(BASS_ErrorGetCode()));
				}
				std::free(buff);
				return new Audio::Channel(-1);
			}

			Audio::Channel* c = new Audio::Channel(val);
			c->data = buff;
			c->name = name;

			c->path = path;
			c->SetVolume(0.2);

			const QWORD word = BASS_ChannelGetLength(val, BASS_POS_BYTE);
			c->length = BASS_ChannelBytes2Seconds(val, word) * 1000;

			c->autoFree = autoFree;
			Channels.push_back(c);

			return c;
		}
	};
}