/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_VIDEO_DATA_H
#define AVG4K_VIDEO_DATA_H

#pragma once

#include <msgpack.hpp>

namespace Average4k::Data::Types
{
	class VideoData {
	public:
		VideoData() = default;

		int width = 1920;
		int height = 1080;

		MSGPACK_DEFINE_ARRAY(width, height);
	};
}

#endif