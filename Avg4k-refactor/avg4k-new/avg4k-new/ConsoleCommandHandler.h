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
		std::vector<std::string> commands{};
		ConsoleCommandHandler()
		{
			commands.push_back("help");
			commands.push_back("performgraph");
		}
		virtual void Handle(std::string cmd);
	};
}