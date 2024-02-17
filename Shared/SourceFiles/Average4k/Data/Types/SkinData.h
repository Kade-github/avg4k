/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_SKIN_DATA_H
#define AVG4K_SKIN_DATA_H

#pragma once

#include <msgpack.hpp>
#include <string>

namespace Average4k::Data::Types
{
	class SkinData {
	public:
		SkinData() {
			name = "Default";

			relativePath = true;
			path = "Assets/Skins/Default";
		
		}
		std::string name;

		bool relativePath;
		std::string path;

		MSGPACK_DEFINE_ARRAY(name, relativePath, path);
	};
}

#endif