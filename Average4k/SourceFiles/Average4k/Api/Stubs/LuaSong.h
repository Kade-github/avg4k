/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_SONG_H
#define AVG4K_LUA_SONG_H

#pragma once

#include <AvgEngine/External/Bass/BASS.h>
#include "../../A4kGame.h"

#define SOL_NO_EXCEPTIONS 1
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
			_base = AvgEngine::External::BASS::CreateChannel(name, A4kGame::gameInstance->skin.GetPath(audioPath));
		}

		bool isValid()
		{
			return _base != NULL;
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
				sol::meta_function::garbage_collect,
				"Play", &LuaSong::Play,
				"Stop", &LuaSong::Stop,
				"SetVolume", &LuaSong::SetVolume,
				"GetVolume", &LuaSong::GetVolume,
				"SetSpeed", &LuaSong::SetSpeed,
				"GetSpeed", &LuaSong::GetSpeed,
				"isValid", &LuaSong::isValid,
				sol::destructor(&LuaSong::Destruct)
			);
		}

	};
}

#endif