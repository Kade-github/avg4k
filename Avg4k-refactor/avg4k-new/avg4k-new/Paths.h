#pragma once
#include "includes.h"
#include <shlobj.h>

namespace AvgEngine::Utils
{
	class Paths
	{
	public:
		static std::string getAppData(std::string p)
		{
            PWSTR   docs;

            HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &docs);


            std::string bangerPath = "";


            if (SUCCEEDED(hr)) {
                std::wstring str = std::wstring(docs);

                bangerPath = std::string(str.begin(), str.end()) + "/" + p + "/";

                CreateDirectory(Helpers::s2ws((bangerPath)).c_str(), NULL);

                if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != ERROR_SUCCESS)
                {
                    bangerPath = "/data/";
                }
            }

            return bangerPath;
		}
	};
}
