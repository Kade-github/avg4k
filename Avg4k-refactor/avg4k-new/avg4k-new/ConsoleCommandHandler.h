#pragma once
#include "includes.h"

namespace AvgEngine::Debug
{
	inline constexpr auto hash_djb2a(const std::string_view sv) {
		unsigned long hash{ 5381 };
		for (unsigned char c : sv) {
			hash = ((hash << 5) + hash) ^ c;
		}
		return hash;
	}

	class ConsoleCommandHandler {
	public:
		virtual void Handle(std::string cmd);
	};
}