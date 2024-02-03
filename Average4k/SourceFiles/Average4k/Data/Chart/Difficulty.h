/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_DIFFICULTY_H
#define _AVG4K_DIFFICULTY_H

#include <iostream>
#include <vector>

namespace Average4k::Data::Chart
{
	enum NoteType {
		Tap = 0,
		Head = 1,
		Tail = 3,
		Mine = 4,
		Fake = 5
	};

	struct Note {
		float beat;
		int lane;

		NoteType type;
	};

	class Difficulty
	{
	public:
		std::string name;
		std::string charter;

		std::vector<Note> notes;

		int rating;
	};
}

#endif