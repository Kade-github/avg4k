#pragma once
#include "includes.h"
#include "ConsoleCommandHandler.h"

struct Bind {
	int key;
	std::string bind_name;
	std::string command;
	int eventId = -1;
};

class Avg4kCmdHandler : public AvgEngine::Debug::ConsoleCommandHandler
{
public:
	static Avg4kCmdHandler* instance;
	std::vector<Bind> binds;
	void load_binds();

	Avg4kCmdHandler() : AvgEngine::Debug::ConsoleCommandHandler()
	{
		instance = this;
		commands.push_back("reload");
		commands.push_back("bind");
		commands.push_back("objects");
		commands.push_back("dontcare");
		commands.push_back("lua");
		commands.push_back("basserror");
		commands.push_back("hitboxes");

		load_binds();
	}
	void Handle(std::string cmd) override;
};

