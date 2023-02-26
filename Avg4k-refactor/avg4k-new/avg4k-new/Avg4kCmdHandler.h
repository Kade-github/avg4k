#pragma once
#include "includes.h"
#include "ConsoleCommandHandler.h"
class Avg4kCmdHandler : public AvgEngine::Debug::ConsoleCommandHandler
{
	void Handle(std::string cmd) override;
};

