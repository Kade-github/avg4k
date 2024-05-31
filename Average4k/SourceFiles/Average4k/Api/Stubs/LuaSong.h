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

		/// <summary>
		/// Constructor for the song
		/// </summary>
		/// <param name="name">The name of the channel</param>
		/// <param name="audioPath">The path of the audio</param>
		LuaSong(std::string name, std::string audioPath)
		{
			_base = AvgEngine::External::BASS::CreateChannel(name, A4kGame::gameInstance->skin.GetPath(audioPath), false);
		}

		/// <summary>
		/// Check if the song is valid
		/// </summary>
		bool isValid()
		{
			return _base != NULL;
		}

		/// <summary>
		/// Set the song to loop
		/// </summary>
		/// <param name="loop">If the song should loop</param>
		void SetLoop(bool loop)
		{
			if (_base != NULL)
				_base->Repeat(loop);
		}

		/// <summary>
		/// Play the song
		/// </summary>
		/// <param name="restart">If the song should start from the beginning</param>
		void Play(bool restart = false)
		{
			if (_base != NULL)
				_base->Play(restart);
		}

		/// <summary>
		/// Stop the song
		/// </summary>
		void Stop()
		{
			if (_base != NULL)
				_base->Stop();
		}

		/// <summary>
		/// Set the volume of the song
		/// </summary>
		/// <param name="volume">The volume</param>
		void SetVolume(float volume)
		{
			if (_base != NULL)
				_base->SetVolume(volume);
		}

		/// <summary>
		/// Get the volume of the song
		/// </summary>
		/// <returns>The volume</returns>
		float GetVolume()
		{
			if (_base != NULL)
				return _base->volume;
			return 0;
		
		}

		/// <summary>
		/// Set the speed of the song
		/// </summary>
		/// <param name="speed">The speed as a multiplier (1.25 etc)</param>
		void SetSpeed(float speed)
		{
			if (_base != NULL)
				_base->RateChange(speed);
		}

		/// <summary>
		/// Get the speed of the song
		/// </summary>
		/// <returns>The speed</returns>
		float GetSpeed()
		{
			if (_base != NULL)
				return _base->rate;
			return 0;
		}

		/// <summary>
		/// Get the position of the song
		/// </summary>
		/// <returns>The position</returns>
		float GetPosition()
		{
			if (_base != NULL)
				return _base->GetPos();
			return 0;
		}

		/// <summary>
		/// Set the position of the song
		/// </summary>
		/// <param name="pos">The position</param>
		void SetPosition(float pos)
		{
			if (_base != NULL)
				_base->SetPos(pos);
		}

		/// <summary>
		/// Destructor for the song
		/// </summary>
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