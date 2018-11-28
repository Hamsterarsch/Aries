#pragma once
#include <wrl/client.h>
#include "IWindow.h"
#include <memory>
#include "ISurfacePolicy.h"
#include "IGAPIFactory.h"

class FWinWindow : public IWindow
{
public:
	FWinWindow(IGAPIFactory *pFactory, HINSTANCE hInstance, WNDPROC pfWndProc, LPCWSTR WndClassName, UINT Width = 1920, UINT Height = 1080, LPCWSTR WindowName = L"Default", HWND Parent = nullptr);
	~FWinWindow();
		
	FWinWindow(FWinWindow &Other) = delete;
	FWinWindow &operator=(FWinWindow &Rhs) = delete;

	FWinWindow(FWinWindow &&Other) = default;
	FWinWindow &operator=(FWinWindow &&Rhs) = default;

	void Show(int nCmdShow){ ShowWindow(m_hWnd, nCmdShow); }

protected:
	void RegisterWindowClass(HINSTANCE hInstance, WNDPROC pfWndProc, LPCWSTR WndClassName) const;

	HINSTANCE m_hInstance;
	LPCWSTR m_WndClassName;
	HWND m_hWnd;
	std::unique_ptr<ISurfacePolicy> m_pSurfacePolicy;
	

};
