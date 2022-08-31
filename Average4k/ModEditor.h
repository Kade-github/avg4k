#pragma once
#include "Menu.h"
#include "ReceptorObject.h"
#include "ModManager.h"
class ModEditor : public Menu
{
public:
	void create() override;
	void update(Events::updateEvent event) override;
	void imguiUpdate(float elapsed) override;

	ModManager manager;

	std::vector<ReceptorObject*> receptors;
};

