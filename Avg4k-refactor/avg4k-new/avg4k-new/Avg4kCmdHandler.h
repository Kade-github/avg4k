#pragma once
#include "includes.h"
#include "ConsoleCommandHandler.h"
class Avg4kCmdHandler : public AvgEngine::Debug::ConsoleCommandHandler
{
public:
	Avg4kCmdHandler() : AvgEngine::Debug::ConsoleCommandHandler()
	{
		commands.push_back("reload");
		commands.push_back("bind");
		commands.push_back("objects");
		commands.push_back("dontcare");
		commands.push_back("lua");
		commands.push_back("basserror");
		commands.push_back("hitboxes");
	}
	void Handle(std::string cmd) override;
};

