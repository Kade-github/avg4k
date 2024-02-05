/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_SONG_H
#define AVG4K_LUA_SONG_H

#pragma once

#include <AvgEngine/External/Bass/BASS.h>
#include "../../A4kGame.h"

#define SOL_USE_LUA_HPP
#include <sol/sol.hpp>

namespace Average4k::Api::Stubs
{

	class LuaSong {
		AvgEngine::Audio::Channel* _base;
	public:
		LuaSong()
		{
			_base = NULL;
		}
		LuaSong(AvgEngine::Audio::Channel* base)
		{
			_base = base;
		}

		LuaSong(std::string name, std::string audioPath)
		{
			_base = AvgEngine::External::BASS::CreateChannel(name, A4kGame::gameInstance->skin.GetPath(audioPath), false);
		}

		bool isValid()
		{
			return _base != NULL;
		}

		void SetLoop(bool loop)
		{
			if (_base != NULL)
				_base->Repeat(loop);
		}

		void Play(bool restart = false)
		{
			if (_base != NULL)
				_base->Play(restart);
		}

		void Stop()
		{
			if (_base != NULL)
				_base->Stop();
		}

		void SetVolume(float volume)
		{
			if (_base != NULL)
				_base->SetVolume(volume);
		}

		float GetVolume()
		{
			if (_base != NULL)
				return _base->volume;
			return 0;
		
		}

		void SetSpeed(float speed)
		{
			if (_base != NULL)
				_base->RateChange(speed);
		}

		float GetSpeed()
		{
			if (_base != NULL)
				return _base->rate;
			return 0;
		}

		float GetPosition()
		{
			if (_base != NULL)
				return _base->GetPos();
			return 0;
		}

		void SetPosition(float pos)
		{
			if (_base != NULL)
				_base->SetPos(pos);
		}

		void Destruct()
		{
			if (_base != NULL)
			{
				AvgEngine::External::BASS::RemoveChannel(_base->id);
				delete _base;
			}
		}
		static void Register(sol::state& state)
		{
			state.new_usertype<LuaSong>("Song",
				sol::constructors<LuaSong(std::string, std::string)>(),
				"Play", &LuaSong::Play,
				"Stop", &LuaSong::Stop,
				"volume", sol::property(&LuaSong::GetVolume, &LuaSong::SetVolume),
				"speed", sol::property(&LuaSong::GetSpeed, &LuaSong::SetSpeed),
				"time", sol::property(&LuaSong::GetPosition, &LuaSong::SetPosition),
				"Repeat", &LuaSong::SetLoop,
				"Destruct", &LuaSong::Destruct,
				"isValid", sol::readonly_property(&LuaSong::isValid)
			);
		}

	};
}

#endif