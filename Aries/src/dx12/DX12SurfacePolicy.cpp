#include "dx12\DX12Factory.h"
#include "Error.h"
#include "dx12/DX12SurfacePolicy.h"

FDX12SurfacePolicy::FDX12SurfacePolicy(FDX12Factory *pFactory, HWND hWnd, UINT Width, UINT Height, bool bIsWindowed, UINT FrameCount) :
	m_RTVHeap{ pFactory->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FrameCount, false },
	m_vRenderTargets(FrameCount)
{
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FsDesc{};
	FsDesc.Windowed = true;

	DXGI_SWAP_CHAIN_DESC1 SwDesc{};
	SwDesc.Width = 0;
	SwDesc.Height = 0;
	SwDesc.Stereo = false;
	SwDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwDesc.SampleDesc.Count = 1;
	SwDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwDesc.BufferCount = FrameCount;
		
	ARI_THROW_IF_FAILED(
		pFactory->GetDXGIFactory()->CreateSwapChainForHwnd( pFactory->GetCommonCmdQ(), hWnd, &SwDesc, &FsDesc, nullptr, m_pSwapChain.GetAddressOf() ),
	"Could not create dx12 window swapchain.");
			
	for (UINT BufferIndex{ 0 }; BufferIndex < m_vRenderTargets.size(); ++BufferIndex)
	{
		ARI_THROW_IF_FAILED(
			m_pSwapChain->GetBuffer(BufferIndex, IID_PPV_ARGS(&m_vRenderTargets.at(BufferIndex))),
			"Could not aquire dx12 window swapchain buffer.");

		pFactory->GetDevice()->CreateRenderTargetView(m_vRenderTargets.at(BufferIndex).Get(), nullptr, m_RTVHeap.GetHandleCPU(BufferIndex));


	}

	// This sample does not support fullscreen transitions.
	//ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));


}
