/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_SAVE_DATA_H
#define AVG4K_SAVE_DATA_H

#pragma once

#include <fstream>
#include <filesystem>

#include "Types/SkinData.h"
#include "Types/GameplayData.h"

#include "../Defines.h"

namespace Average4k::Data
{
	class SaveData {
	public:
		SaveData() = default;
		std::string header = A_SHEADER;

		Types::SkinData skinData;
		Types::KeybindData keybindData;
		Types::GameplayData gameplayData;

		void Save(std::string file)
		{
			SaveData c = *this;
			std::ofstream output(file, std::ios::out | std::ios::binary);
			msgpack::pack(output, c);
			output.close();
		}

		static SaveData Load(std::string file)
		{
			if (!std::filesystem::exists(file))
				return SaveData();

			std::ifstream ifs(file, std::ifstream::in | std::ios::binary);
			std::stringstream buffer;
			buffer << ifs.rdbuf();
			msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());
			ifs.close();
			SaveData s;

			msgpack::object obj = msgpack::object(upd.get());

			obj.convert(s);

			return s;
		}

		MSGPACK_DEFINE_ARRAY(header, skinData, keybindData, gameplayData);
	};
}

#endif