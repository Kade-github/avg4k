#pragma once
#include "includes.h"
#include "SparrowAtlas.h"
#include "Menu.h"
class OffsetTester :
	public Menu
{
public:
	SparrowAtlas* gf;
	SparrowAtlas* bf;

	int type = 0;

	int index = 0;

	void playAnim()
	{
		int i = 0;
		if (type == 0)
		{
			for (auto const& [key, val] : gf->frames)
			{
				if (i == index)
				{
					gf->play(key, 24, 0, 0);
					break;
				}
				i++;
			}
		}
		else
		{
			for (auto const& [key, val] : bf->frames)
			{
				if (i == index)
				{
					bf->play(key, 24, 0, 0);
					break;
				}
				i++;
			}
		}
	}

	Text* t;

	OffsetTester() {

		t = new Text(0, 40, "", 24);
		t->create();

		gf = new SparrowAtlas("assets/holiday/GF_assets.png", "assets/holiday/GF_assets.xml", 24, "GF Dancing Beat");
		gf->x = (Game::gameWidth / 2) - 500;
		gf->y = (Game::gameHeight / 2) - 250;
		gf->loop = true;
		gf->showOnion = true;

		bf = new SparrowAtlas("assets/holiday/BOYFRIEND.png", "assets/holiday/BOYFRIEND.xml", 24, "BF idle dance");
		bf->x = (Game::gameWidth / 2) - 500;
		bf->y = (Game::gameHeight / 2) - 250;
		bf->loop = true;
		bf->showOnion = true;
		bf->visible = false;

		bf->scale = 0.8;

		gf->scale = 0.8;

		playAnim();
	}


	void update(Events::updateEvent event) {
		gf->draw();
		bf->draw();

		if (type == 0)
			t->setText(std::to_string(gf->_offsetx) + ":" + std::to_string(gf->_offsety));
		else
			t->setText(std::to_string(bf->_offsetx) + ":" + std::to_string(bf->_offsety));
	}

	void keyDown(SDL_KeyboardEvent event) {
		switch (event.keysym.sym)
		{
		case SDLK_1:
			index--;
			switch (type)
			{
			case 0:
				if (index < 0)
					index = gf->frames.size() - 1;
				break;
			case 1:
				if (index < 0)
					index = bf->frames.size() - 1;
				break;
			}
			playAnim();
			break;
		case SDLK_2:
			index++;
			switch (type)
			{
			case 0:
				if (index > gf->frames.size() - 1)
					index = 0;
				break;
			case 1:
				if (index > bf->frames.size() - 1)
					index = 0;
				break;
			}
			playAnim();
			break;
		case SDLK_LEFT:
			if (type == 0)
				gf->_offsetx--;
			else
				bf->_offsetx--;
			break;
		case SDLK_RIGHT:
			if (type == 0)
				gf->_offsetx++;
			else
				bf->_offsetx++;
			break;
		case SDLK_DOWN:
			if (type == 0)
				gf->_offsety++;
			else
				bf->_offsety++;
			break;
		case SDLK_UP:
			if (type == 0)
				gf->_offsety--;
			else
				bf->_offsety--;
			break;
		case SDLK_RETURN:
			type = type == 1 ? 0 : 1;
			gf->visible = !gf->visible;
			bf->visible = !bf->visible;
			break;
		}
	}
};

