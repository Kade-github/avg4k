/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_CMDHANDLER_H_
#define _AVG4K_CMDHANDLER_H_

#include <AvgEngine/Debug/ConsoleCommandHandler.h>

namespace Average4k::Console
{

	class CmdHandler : public AvgEngine::Debug::ConsoleCommandHandler {
	public:
		static CmdHandler* instance;

		CmdHandler() : AvgEngine::Debug::ConsoleCommandHandler()
		{
			instance = this;

			commands.push_back("reload");
		}

		void Handle(std::string cmd) override;
	};
}
#endif
