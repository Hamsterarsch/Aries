#include "DeviceResources.h"
#include "Error.h"

FDeviceResources::FDeviceResources(HWND hWnd)
{
	AcquireDevice(m_pDevice, m_pDeviceContext);

	AcquireWindowResources(m_pSwapChain, m_pDevice, hWnd);
	AcquireBackbuffer(m_pBackBuffer, m_pBackBufferView, m_BackbufferDesc, m_pSwapChain, m_pDevice);
	AcquireDepthStencil(m_pDepthStencil, m_pDepthStencilView, m_BackbufferDesc, m_pSwapChain, m_pDevice);

	RECT WindowRect{};
	GetWindowRect(hWnd, &WindowRect);
	//m_DepthPrePassBuffer = FDepthBuffer{ m_pDevice.Get(), static_cast<UINT>(WindowRect.right), static_cast<UINT>(WindowRect.bottom), DXGI_FORMAT_R32G32B32A32_FLOAT };

	FitViewportSize(m_pDeviceContext, m_Viewport, m_BackbufferDesc);


}

FDeviceResources::~FDeviceResources()
{
	ReleaseBackbuffer();


}

void FDeviceResources::PresentBackbuffer()
{
	m_pSwapChain->Present(1, 0);


}

void FDeviceResources::GoWindowed(const UINT Width, const UINT Height)
{
	//Release so we can recreate a new buffer with the new properties
	ReleaseBackbuffer();

	auto Hr = m_pSwapChain->ResizeBuffers
	(
		0,							//Number of buffers.Set this to 0 to preserve the existing setting.
		Width, Height,				//Width and height of the swap chain.MUST be set to a non - zero value.For example, match the window size.
		DXGI_FORMAT_UNKNOWN,		//This tells DXGI to retain the current back buffer format.
		0
	);

	if (FAILED(Hr))
	{
		throw(FError{ Hr, "Failed to resize swap chain buffers.", __FILE__, __LINE__ });

	}

	//Recreate the backbuffer with new properties
	AcquireBackbuffer(m_pBackBuffer, m_pBackBufferView, m_BackbufferDesc, m_pSwapChain, m_pDevice);

	if (!m_DepthPrePassBuffer.Resize(m_pDevice.Get(), Width, Height))
	{
		throw(FError{ -1, "Could not resize DepthPrePassTarget", __FILE__, __LINE__ });

	}

	
}

void FDeviceResources::GoFullscreen()
{
	ReleaseBackbuffer();
	auto Hr = m_pSwapChain->ResizeBuffers
	(
		0,
		0, 0,
		DXGI_FORMAT_UNKNOWN,
		0
	);
	AcquireBackbuffer(m_pBackBuffer, m_pBackBufferView, m_BackbufferDesc, m_pSwapChain, m_pDevice);


}

void FDeviceResources::AcquireDevice(ComPtr<ID3D11Device> &out_pDevice, ComPtr<ID3D11DeviceContext> &out_pDeviceContext)
{
	D3D_FEATURE_LEVEL DesiredFeatureLevels[] =
	{/*
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,*/
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1
	};

	UINT DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif

	//Device creation
	auto Hr = D3D11CreateDevice
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		DeviceFlags,
		DesiredFeatureLevels,
		ARRAYSIZE(DesiredFeatureLevels),
		D3D11_SDK_VERSION,
		&out_pDevice,
		&m_DeviceFeatureLevel,
		&out_pDeviceContext
	);

	if (FAILED(Hr))
	{
		throw(FError{ Hr, "Failed to create device resources.", __FILE__, __LINE__ });

	}



}

void FDeviceResources::AcquireWindowResources(ComPtr<IDXGISwapChain> &out_pSwapChain, const ComPtr<ID3D11Device> &pDevice, const HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
	SwapChainDesc.Windowed = true;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.OutputWindow = hWnd;

	//number 2 to support win7, use the device as dxgi device
	ComPtr<IDXGIDevice2> pDXGIDevice;
	pDevice.As(&pDXGIDevice);

	ComPtr<IDXGIAdapter> pAdapter; //Display subsystem abstraction (GPU, vram, digital/analog converter)
	ComPtr<IDXGIFactory> pFactory; //DXGI factory

	auto Hr = pDXGIDevice->GetAdapter(&pAdapter);
	if (SUCCEEDED(Hr))
	{
		//Get the factory that create the adapter		
		pAdapter->GetParent(IID_PPV_ARGS(&pFactory)); //Macro deduces guiid to prevent mismatch of input pointer and required ptr type
		Hr = pFactory->CreateSwapChain(pDevice.Get(), &SwapChainDesc, &out_pSwapChain);
		if (FAILED(Hr))
		{
			throw(FError{ Hr, "Failed to create swap chain.", __FILE__, __LINE__ });

		}
		
	}
	else
	{
		throw(FError{ Hr, "Failed to query adapter for swap chain creation.", __FILE__, __LINE__ });

	}
	


}

void FDeviceResources::AcquireBackbuffer(ComPtr<ID3D11Texture2D> &out_pBackbuffer, ComPtr<ID3D11RenderTargetView> &out_pBackbufferView, D3D11_TEXTURE2D_DESC &out_BufferDesc, const ComPtr<IDXGISwapChain> &pSwapChain, const ComPtr<ID3D11Device> &pDevice)
{
	//Gets the 0th buffer of the Swap Chain, the only one writable with DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL set, representing the current backbuffer.
	//Presenting the values and swapping memory seems to be managed by the swap chain itself. The Format of buffers inside the swap chain is determined at swap chain creation.
	//https://msdn.microsoft.com/de-de/library/windows/desktop/bb174570(v=vs.85).aspx
	auto BufferHr = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&out_pBackbuffer));

	//Create a view of the 0th (back)buffer
	auto ViewHr = pDevice->CreateRenderTargetView(out_pBackbuffer.Get(), nullptr, &out_pBackbufferView);
	   	
	if (FAILED(BufferHr) || FAILED(ViewHr))
	{
		throw(FError{ -1, "Failed to configure backbuffer.", __FILE__, __LINE__ });

	}
	out_pBackbuffer->GetDesc(&out_BufferDesc);
	   	 

}

void FDeviceResources::AcquireDepthStencil
(
	ComPtr<ID3D11Texture2D> &out_pDepthStencil,
	ComPtr<ID3D11DepthStencilView> &out_pDepthStencilView,
	const D3D11_TEXTURE2D_DESC &BackbufferDesc,
	const ComPtr<IDXGISwapChain> &pSwapChain, 
	const ComPtr<ID3D11Device> &pDevice
)
{
	CD3D11_TEXTURE2D_DESC DepthStencilDesc
	{
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		BackbufferDesc.Width,
		BackbufferDesc.Height,
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
	};

	auto BufferHr = pDevice->CreateTexture2D(&DepthStencilDesc, nullptr, &out_pDepthStencil);

	CD3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc{ D3D11_DSV_DIMENSION_TEXTURE2D };
	auto ViewHr = pDevice->CreateDepthStencilView(out_pDepthStencil.Get(), &DepthStencilViewDesc, &out_pDepthStencilView);

	if (FAILED(BufferHr) || FAILED(ViewHr))
	{
		throw(FError{ -1, "Failed to create depth stencil buffer.", __FILE__, __LINE__ });

	}
	

}

void FDeviceResources::ReleaseBackbuffer()
{	
	m_pBackBufferView.Reset();
	m_pBackBuffer.Reset();

	m_pDepthStencilView.Reset();
	m_pDepthStencil.Reset();

	m_pDeviceContext->Flush();
	   	

}

void FDeviceResources::FitViewportSize(ComPtr<ID3D11DeviceContext> &out_pDeviceContext, D3D11_VIEWPORT &out_Viewport, const D3D11_TEXTURE2D_DESC &BackbufferDesc)
{
	//alt set top and left to 0
	ZeroMemory(&out_Viewport, sizeof(out_Viewport));
	out_Viewport.Height = static_cast<float>(BackbufferDesc.Height);
	out_Viewport.Width = static_cast<float>(BackbufferDesc.Width);
	out_Viewport.MinDepth = 0;
	out_Viewport.MaxDepth = 1;

	out_pDeviceContext->RSSetViewports(1, &out_Viewport);


}