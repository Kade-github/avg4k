#pragma once
#include "includes.h"
#include "Channel.h"
namespace Average4k::Lua::Menu
{

	struct LuaChannel {
	public:

		AvgEngine::Audio::Channel* base = NULL;

		std::string path;
		std::string name;

		bool isFreed = false;

		void Free()
		{
			if (base)
			{
				base->Free();
				isFreed = true;
			}
		}

		void Play()
		{
			if (base)
			{
				base->Play();
			}
		}

		void Stop()
		{
			if (base)
			{
				base->Stop();
			}
		}

		float GetPos()
		{
			if (base)
			{
				return base->GetPos();
			}
			return 0;
		}

		void SetPos(float pos)
		{
			if (base)
			{
				base->SetPos(pos);
			}
		}

		float GetVolume()
		{
			if (base)
			{
				return base->volume;
			}
			return 0;
		}

		void SetVolume(float volume)
		{
			if (base)
			{
				base->SetVolume(volume);
			}
		}

		void RateChange(float rate)
		{
			if (base)
			{
				base->RateChange(rate);
			}
		}

		float GetSampleRate()
		{
			if (base)
			{
				return base->SampleRate();
			}
			return 0;
		}

		void ConvertToFX()
		{
			if (base)
			{
				base->ConvertToFX();
			}
		}

		LuaChannel(std::string _path, std::string _name)
		{

		}
	};
}