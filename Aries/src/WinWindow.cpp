#include "Error.h"
#include "WinWindow.h"

FWinWindow::FWinWindow(HINSTANCE hInstance, WNDPROC pfWndProc, LPCWSTR WndClassName, const UINT Width, const UINT Height, LPCWSTR WindowName, HWND Parent) :
	m_hInstance{ hInstance },
	m_WndClassName{ WndClassName }
{
	RegisterWindowClass(hInstance, pfWndProc, m_WndClassName);
	
	m_hWnd = CreateWindowEx(0, m_WndClassName, WindowName, WS_OVERLAPPEDWINDOW, 0, 0, Width, Height, NULL, NULL, hInstance, NULL);
	if (!m_hWnd)
	{
		throw(FError{ -1, "Failed to create window.", __FILE__, __LINE__ });
		
	}	
	//UpdateWindow(m_hWnd);


}

FWinWindow::~FWinWindow()
{
	UnregisterClass(m_WndClassName, m_hInstance);


}

void FWinWindow::RegisterWindowClass(const HINSTANCE hInstance, WNDPROC pfWndProc, const LPCWSTR WndClassName) const
{
	WNDCLASSEX Wcex{};
	Wcex.cbSize = sizeof(decltype(Wcex));
	Wcex.style = CS_HREDRAW | CS_VREDRAW;
	Wcex.hInstance = hInstance;
	Wcex.lpfnWndProc = pfWndProc;
	Wcex.lpszClassName = WndClassName;


	if (!RegisterClassEx(&Wcex))
	{
		throw(FError{ -1, "Could not register window class.", __FILE__, __LINE__ });
		
	}


}