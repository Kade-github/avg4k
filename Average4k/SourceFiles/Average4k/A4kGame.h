/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_GAME_H
#define AVG4K_GAME_H

#pragma once

#include <AvgEngine/Game.h>
#include <AvgEngine/Base/Text.h>

namespace Average4k
{
	class A4kGame : public AvgEngine::Game
	{
	public:
		static A4kGame* gameInstance;

		static float ScaleFactorX;
		static float ScaleFactorY;

		AvgEngine::Base::Text* debugText;
		AvgEngine::Base::Text* debugTextOutlined;
		A4kGame(std::string _t, std::string _v, int w = 1920, int h = 1080);

		void Destroy()
		{

		}

		void Start();

		void Switch() override
		{
			Game::Switch();
			if (!debugText)
			{
				debugText = new AvgEngine::Base::Text(0, -20, "Assets/Fonts/", "Arial.fnt", "", 32);
				debugTextOutlined = new AvgEngine::Base::Text(0, -20, "Assets/Fonts/", "ArialOutlined.fnt", "", 32);
			}
			debugText->tween = &CurrentMenu->tween;
			debugText->eManager = CurrentMenu->eManager;
			debugText->camera = &CurrentMenu->camera;
			debugText->parent = &CurrentMenu->displayRect;
			debugText->parentI = &CurrentMenu->displayRect;
			debugText->camera = &CurrentMenu->camera;
			debugTextOutlined->tween = &CurrentMenu->tween;
			debugTextOutlined->eManager = CurrentMenu->eManager;
			debugTextOutlined->camera = &CurrentMenu->camera;
			debugTextOutlined->parent = &CurrentMenu->displayRect;
			debugTextOutlined->parentI = &CurrentMenu->displayRect;
			debugTextOutlined->camera = &CurrentMenu->camera;
		}

		void DrawDebugText(float x, float y, std::string text, int size)
		{
			debugText->transform.x = x;
			debugText->transform.y = y;
			debugText->SetText(text);
			debugText->SetSize(size);
			debugText->draw();
		}

		void DrawOutlinedDebugText(float x, float y, std::string text, int size)
		{
			debugTextOutlined->transform.x = x;
			debugTextOutlined->transform.y = y;
			debugTextOutlined->SetText(text);
			debugTextOutlined->SetSize(size);
			debugTextOutlined->draw();
		}
	};
}

#endif