#pragma once
#include <algorithm>
#include <vector>
namespace AvgEngine::Utils
{
	class StringTools
	{
	public:
		static bool Contains(std::string& search, std::string find)
		{
			return search.find(find) != std::string::npos;
		}

        static const wchar_t* GetWC(const char* c)
        {
            const size_t cSize = strlen(c) + 1;
            wchar_t* wc = new wchar_t[cSize];
            mbstowcs(wc, c, cSize);

            return wc;
        }

        static std::vector<std::string> Split(std::string s, std::string delimiter) {
            size_t pos_start = 0, pos_end, delim_len = delimiter.length();
            std::string token;
            std::vector<std::string> res;

            while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
                token = s.substr(pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                res.push_back(token);
            }

            res.push_back(s.substr(pos_start));
            return res;
        }

        static void ToLower(std::string& s)
		{
            std::transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c) { return std::tolower(c); });
		}
	};
}