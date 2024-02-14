/*
    Avg Dev Team
    2021-2024 Average4k
*/

#ifndef AVG4K_FILE_DIAG_H
#define AVG4K_FILE_DIAG_H

#include <windows.h>
#include <string>
#include <ShlObj.h>

#pragma once
namespace Average4k::Helpers
{
    class Files {
    public:
        /// <summary>
        /// Returns the file path of the file selected by the user as an "Open File" dialog.
        /// </summary>
        /// <param name="filters">The filters to impose on the user</param>
        /// <returns>The file path</returns>
        static inline std::string GetFile(LPCWSTR filters)
        {
            // thing
            OPENFILENAME ofn;

            // file name
            wchar_t szFile[MAX_PATH] = { 0 };

            // make it work
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);

            // args
            ofn.lpstrFile = szFile;
            ofn.lpstrFilter = filters;
            ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; // dont change our working directory PLEASE

            // Open As

            if (GetOpenFileName(&ofn) == TRUE)
            {
                std::wstring ws(szFile);
                std::string str(ws.begin(), ws.end());
                return str;
            }
            else
                return "";

        }

        static inline std::string GetFolder()
        {
			// thing
			BROWSEINFO bi = { 0 };
			bi.lpszTitle = L"Browse for folder...";
			LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

			// get the name of the folder
			wchar_t path[MAX_PATH];
            if (pidl != 0)
            {
				SHGetPathFromIDList(pidl, path);
				// free memory used
				IMalloc* imalloc = 0;
                if (SUCCEEDED(SHGetMalloc(&imalloc)))
                {
					imalloc->Free(pidl);
					imalloc->Release();
				}
				std::wstring ws(path);
				std::string str(ws.begin(), ws.end());
				return str;
			}
			else
				return "";
		}

        /// <summary>
        /// Returns the file path of the file selected by the user as an "Save File" dialog.
        /// </summary>
        /// <param name="filters">The filters to impose on the user</param>
        /// <param name="ext">The extenstion to save as</param>
        /// <returns>The file path they want to save to</returns>
        static inline std::string SaveFile(LPCWSTR filters, LPCWSTR ext)
        {
            // thing
            OPENFILENAME ofn;

            // file name
            wchar_t szFile[MAX_PATH] = { 0 };

            // make it work
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);

            // args
            ofn.lpstrDefExt = ext;
            ofn.lpstrFile = szFile;
            ofn.lpstrFilter = filters;
            ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR; // dont change our working directory PLEASE

            // Save As

            if (GetSaveFileName(&ofn) == TRUE)
            {
                std::wstring ws(szFile);
                std::string str(ws.begin(), ws.end());
                return str;
            }
            else
                return "";
        }

        static inline bool DoesFileExist(const std::string& name) {
            struct stat buffer;
            return (stat(name.c_str(), &buffer) == 0);
        }


    };
}

#endif