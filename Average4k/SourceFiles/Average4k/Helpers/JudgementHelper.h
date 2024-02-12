/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_JUDGEMENT_HELPER_H
#define AVG4K_JUDGEMENT_HELPER_H

#pragma once

#include <string>
#include <map>

namespace Average4k::Helpers
{
	class JudgementHelper
	{
	public:
		static std::map<float, std::string> JudgementMap;

		static void Clear()
		{
			JudgementMap.clear();
		}

		static void AddJudgement(float difference, std::string judgement)
		{
			JudgementMap[difference] = judgement;
		}

		static void RemoveJudgement(float difference)
		{
			JudgementMap.erase(difference);
		}

		static std::string GetJudgement(float difference)
		{
			float abs = std::abs(difference * 1000);
			std::string judge = "Miss";
			for (auto& judgement : JudgementMap)
			{
				if (abs <= judgement.first)
				{
					judge = judgement.second;
				}
			}
			return judge;
		}
		
	};
}

#endif