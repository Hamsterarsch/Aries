#include <d3d12sdklayers.h>
#include "Error.h"
#include "dx12/DX12Factory.h"
#include "dx12/DX12SurfacePolicy.h"
#include "WinWindow.h"

std::shared_ptr<FDX12Factory> FDX12Factory::s_pInstance{};

std::shared_ptr<IGAPIFactory> FDX12Factory::GetInstance()
{
	
	if (!s_pInstance)
	{
		s_pInstance = std::shared_ptr<FDX12Factory>(new FDX12Factory());

	}
	return s_pInstance;
	
	
}

FDX12Factory::FDX12Factory()
{
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> pDebugController;
	ARI_ASSERT(
		SUCCEEDED( D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)) ),
	"Could not create dx12 debug interface.");

	pDebugController->EnableDebugLayer();

#endif

	ARI_ASSERT(
		SUCCEEDED( CreateDXGIFactory1(IID_PPV_ARGS(&m_pDXGIFactory)) ),
	"Could not create dxgi factory (dx12).");

	if (m_bUseWarpDevice)
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ARI_THROW_IF_FAILED(
			m_pDXGIFactory->EnumWarpAdapter( IID_PPV_ARGS(&pWarpAdapter) ),
		"Could not create dx12 warp adapter.");

		ARI_THROW_IF_FAILED(
			D3D12CreateDevice( pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice) ),
		"Could not create dx12 warp device.");
	
	}
	else
	{
		ComPtr<IDXGIAdapter1> pHardwareAdapter{};
				
		//find an adapter supporting dx12 to create the device
		for (UINT AdapterIndex{ 0 }; ; ++AdapterIndex)
		{
			if (DXGI_ERROR_NOT_FOUND == m_pDXGIFactory->EnumAdapters1(AdapterIndex, pHardwareAdapter.GetAddressOf()))
			{
				continue;

			}

			if( SUCCEEDED( D3D12CreateDevice(pHardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice) )) )
			{
				break;

			}
			pHardwareAdapter->Release();

		}
		ARI_ASSERT(m_pDevice, "Could not create dx12 device.");
		
	}

	D3D12_COMMAND_QUEUE_DESC CommonCmdQDesc{};
	CommonCmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommonCmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ARI_THROW_IF_FAILED(
		m_pDevice->CreateCommandQueue( &CommonCmdQDesc, IID_PPV_ARGS(&m_pFactoryCmdQ) ),
	"Could not create dx12 common cmd queue.");


	ARI_THROW_IF_FAILED(
		m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommonCmdQAlloc)),
	"Could not create dx12 command allocator.");


}

std::unique_ptr<IWindow> FDX12Factory::MakeWindow(UINT Width, UINT Height, LPCWSTR pWindowName, WNDPROC pfnWndProc, LPCWSTR pClassName)
{
	//dx only windows compatible
	return std::make_unique<FWinWindow>(this, GetModuleHandle(nullptr), pfnWndProc, pClassName, Width, Height, pWindowName);


}