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
		VideoData() {
			fullscreen = false;
			borderless = false;

			width = 1920;
			height = 1080;
		}

		bool fullscreen;
		bool borderless;

		int width;
		int height;

		MSGPACK_DEFINE_ARRAY(fullscreen, borderless, width, height);
	};
}

#endif