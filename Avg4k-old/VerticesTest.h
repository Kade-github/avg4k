#pragma once
#include "includes.h"
#include "Menu.h"
class VerticesTest : public Menu {
	void create() override;

	void imguiUpdate(float elapsed) override;

	void update(Events::updateEvent event) override;
};