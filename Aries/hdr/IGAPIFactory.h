#pragma once
#include "IWindow.h"


enum class EAPIType : unsigned short
{
	DX12

};

enum class EOSType : unsigned short
{
	Windows

};

class IGAPIFactory
{
public:
	virtual EAPIType GetApiType() const noexcept = 0;

	virtual std::unique_ptr<IWindow>		MakeWindow(UINT Width, UINT Height, LPCWSTR pWindowName, WNDPROC pfnWndProc, LPCWSTR pClassName) = 0;


};
