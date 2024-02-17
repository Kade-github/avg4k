/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_AUDIO_DATA_H
#define AVG4K_AUDIO_DATA_H

#pragma once

#include <msgpack.hpp>

namespace Average4k::Data::Types
{
	class AudioData {
	public:
		float volume;
		float sfxVolume;

		AudioData() {
			volume = 1.0f;
			sfxVolume = 1.0f;
		}

		MSGPACK_DEFINE_ARRAY(volume, sfxVolume);
	};
}

#endif