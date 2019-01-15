#include "Error.h"
#include "dx12/DX12Factory.h"
#include "dx12/DX12SurfacePolicy.h"
#include "IGAPIFactory.h"
#include "WinWindow.h"


FWinWindow::FWinWindow(IGAPIFactory *pFactory, HINSTANCE hInstance, WNDPROC pfWndProc, LPCWSTR WndClassName, const UINT Width, const UINT Height, LPCWSTR WindowName, HWND Parent) :
	m_hInstance{ hInstance },
	m_WndClassName{ WndClassName }
{
	RegisterWindowClass(hInstance, pfWndProc, m_WndClassName);
	
	m_hWnd = CreateWindowEx(0, m_WndClassName, WindowName, WS_OVERLAPPEDWINDOW, 0, 0, Width, Height, NULL, NULL, hInstance, NULL);
	ARI_ASSERT(m_hWnd, "Failed to create window");
	//UpdateWindow(m_hWnd);

	switch (pFactory->GetApiType())
	{
	case EAPIType::DX12:
		m_pSurfacePolicy = std::make_unique<FDX12SurfacePolicy>(static_cast<FDX12Factory *>(pFactory), m_hWnd, Width, Height, true, 2);
		break;
	default:
		ARI_THROW_ERROR(-1, "FWinWindow:: api not implemented.");
		return;
	}

		
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

	ARI_ASSERT(RegisterClassEx(&Wcex), "Could not register window class.");


}