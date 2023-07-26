#pragma once
#include "includes.h"
#include "Menu.h"

class Editor : public AvgEngine::Base::Menu {
public:
	Editor() {

	}


	void load() override;

	void draw() override;
};