/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_SKIN_DATA_H
#define AVG4K_SKIN_DATA_H

#pragma once

#include <msgpack.hpp>

namespace Average4k::Data::Types
{
	class SkinData {
	public:
		SkinData() = default;
		const char* name = "DefaultArrow";
		const char* script = "Default.lua";

		MSGPACK_DEFINE_ARRAY(name, script);
	};
}

#endif