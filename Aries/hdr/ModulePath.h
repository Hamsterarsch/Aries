#pragma once
#include <WinBase.h>
#include <string>

class FModulePath
{
public:
	static  std::string MakeExeRelative(const char *Path)
	{
		wchar_t WcPathBuffer[256]{};
		GetModuleFileName(nullptr, WcPathBuffer, 256);

		char MbPathBuffer[256]{};
		size_t Converted{};
		wcstombs_s(&Converted, MbPathBuffer, WcPathBuffer, 256);

		std::string Str{ MbPathBuffer };
		

		for (auto Itr = Str.rbegin(); Itr != Str.rend(); ++Itr)
		{
			if (*Itr == '\\')
			{
				break;

			}

			Str.pop_back();


		}

		Str.append(Path);
		return Str;


	}

	static std::wstring MakeExeRelative(const wchar_t *Path)
	{
		wchar_t WcPathBuffer[256]{};
		GetModuleFileName(nullptr, WcPathBuffer, 256);

		std::wstring Str{ WcPathBuffer };

		for (auto Itr = Str.rbegin(); Itr != Str.rend(); ++Itr)
		{
			if (*Itr == '\\')
			{
				break;

			}

			Str.pop_back();


		}

		Str.append(Path);
		return Str;


	}


};
