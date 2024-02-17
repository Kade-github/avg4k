/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_GAMEPLAY_DATA_H
#define AVG4K_GAMEPLAY_DATA_H

#pragma once
#include <string>
#include <msgpack.hpp>
#include <GLFW/glfw3.h>

namespace Average4k::Data::Types
{
	class KeybindData {
	public:
		KeybindData() {
			key0 = GLFW_KEY_D;
			key1 = GLFW_KEY_F;
			key2 = GLFW_KEY_J;
			key3 = GLFW_KEY_K;

			keyPause = GLFW_KEY_ESCAPE;
			keyRestart = GLFW_KEY_GRAVE_ACCENT;
		}

		int key0;
		int key1;
		int key2;
		int key3;

		int keyPause;
		int keyRestart;

		MSGPACK_DEFINE_ARRAY(key0, key1, key2, key3, keyPause, keyRestart);
	};

	class GameplayData {
	public:
		GameplayData() {
			useCmod = true;
			downscroll = false;

			constantMod = 700;
			multiplierMod = 1.0f;

			startTime = 2.0f;

			backgroundDim = 0.65f;
			noteSpace = 1.1f;

			noteskin = "AArrow";
		}

		bool useCmod;
		bool downscroll;

		int constantMod;
		float multiplierMod;

		float startTime;

		float backgroundDim;
		float noteSpace;

		std::string noteskin;

		MSGPACK_DEFINE_ARRAY(useCmod, constantMod, multiplierMod, noteskin, backgroundDim, downscroll, noteSpace);
	};
}

#endif