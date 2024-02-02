/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_GAME_H
#define AVG4K_GAME_H

#pragma once
#include <AvgEngine/Game.h>
#include <AvgEngine/Base/Text.h>

#include "Data/SaveData.h"
#include "Skin/Skin.h"

namespace Average4k
{
	class A4kGame : public AvgEngine::Game
	{
	public:
		Data::SaveData saveData;
		Skin skin;
		static A4kGame* gameInstance;

		AvgEngine::Base::Text* debugText;
		AvgEngine::Base::Text* debugTextOutlined;
		A4kGame(std::string _t, std::string _v, int w = 1920, int h = 1080);

		void Destroy();

		void Start();

		void Switch() override
		{
			if (CurrentMenu != NULL)
			{
				for (AvgEngine::Base::GameObject* obj : CurrentMenu->GameObjects)
				{
					if (!obj->dontDelete)
						delete obj;
				}
				CurrentMenu->GameObjects.clear();
			}
			AvgEngine::Base::Menu* lastMenu = CurrentMenu;
			CurrentMenu = NextMenu;
			CurrentMenu->eManager = &eManager;
			eManager.Clear();
			if (lastMenu != NULL)
			{
				lastMenu->tween.Clear();
				delete lastMenu;
			}
			CurrentMenu->load();
			AvgEngine::Render::Display::defaultShader->setProject(CurrentMenu->camera.projection);
			if (!debugText)
			{
				debugText = skin.CreateText("ArialUnicode.fnt", 32);
				debugTextOutlined = skin.CreateText("ArialUnicodeOutlined.fnt", 32);
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