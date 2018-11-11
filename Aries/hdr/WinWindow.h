#pragma once
#include <wrl/client.h>


class FWinWindow
{
public:
	FWinWindow(HINSTANCE hInstance, WNDPROC pfWndProc, LPCWSTR WndClassName, UINT Width = 1920, UINT Height = 1080, LPCWSTR WindowName = L"Default", HWND Parent = nullptr);
	~FWinWindow();
	
	FWinWindow(FWinWindow &&Other) = default;
	FWinWindow &operator=(FWinWindow &&Rhs) = default;

	HWND GetHandle() const { return m_hWnd; }
	void Show(int nCmdShow){ ShowWindow(m_hWnd, nCmdShow); }

private:
	void RegisterWindowClass(HINSTANCE hInstance, WNDPROC pfWndProc, LPCWSTR WndClassName) const;

	HINSTANCE m_hInstance;
	LPCWSTR m_WndClassName;
	HWND m_hWnd;
	

};
