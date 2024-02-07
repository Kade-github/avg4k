/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_GAMEPLAY_DATA_H
#define AVG4K_GAMEPLAY_DATA_H

#pragma once

#include <msgpack.hpp>
#include <GLFW/glfw3.h>

namespace Average4k::Data::Types
{
	class KeybindData {
	public:
		KeybindData() = default;

		int key0 = GLFW_KEY_D;
		int key1 = GLFW_KEY_F;
		int key2 = GLFW_KEY_J;
		int key3 = GLFW_KEY_K;

		int keyPause = GLFW_KEY_ESCAPE;
		int keyRestart = GLFW_KEY_GRAVE_ACCENT;

		MSGPACK_DEFINE_ARRAY(key0, key1, key2, key3, keyPause, keyRestart);
	};

	class GameplayData {
	public:
		GameplayData() = default;

		bool useCmod = true;

		int constantMod = 700;
		float multiplierMod = 1.0;

		std::string noteskin = "Arrow";

		MSGPACK_DEFINE_ARRAY(useCmod, constantMod, multiplierMod, noteskin);
	};
}

#endif