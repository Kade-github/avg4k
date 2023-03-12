#pragma once
#include "includes.h"
#include "ConsoleCommandHandler.h"
class Avg4kCmdHandler : public AvgEngine::Debug::ConsoleCommandHandler
{
public:
	Avg4kCmdHandler() : AvgEngine::Debug::ConsoleCommandHandler()
	{
		commands.push_back("reload");
#ifdef _DEBUG
		commands.push_back("lua");
#endif
	}
	void Handle(std::string cmd) override;
};

