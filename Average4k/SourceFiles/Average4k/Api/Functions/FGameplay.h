/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_GAMEPLAY_H
#define AVG4K_F_GAMEPLAY_H

#pragma once

#include <string>

namespace Average4k::Api::Functions
{
	class FGameplay {
	public:
		/// <summary>
		/// Set if the game should rotate receptors
		/// </summary>
		/// <param name="rotate">If the game should rotate the receptors</param>
		static void RotateReceptors(bool rotate);
		/// <summary>
		/// Set the note size (as the texture size)
		/// </summary>
		/// <param name="width">The width of the note</param>
		/// <param name="height">The height of the note</param>
		static void SetNoteSize(float width, float height);
	};
}

#endif