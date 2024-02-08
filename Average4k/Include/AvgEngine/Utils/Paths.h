/*
    Copyright 2021-2023 AvgEngine - Kade

    Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef PATHS_H
#define PATHS_H

#pragma once
#include <AvgEngine/Utils/StringTools.h>
#include <iostream>
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

                CreateDirectory(StringTools::S2ws((bangerPath)).c_str(), NULL);

                if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != ERROR_SUCCESS)
                {
                    bangerPath = "/data/";
                }
            }

            return bangerPath;
		}

        static bool pathExists(std::string path)
        {
            return std::filesystem::exists(path);
        }
	};
}

#endif // !PATHS_H