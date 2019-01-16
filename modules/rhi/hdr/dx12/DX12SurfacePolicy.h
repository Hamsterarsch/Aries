#pragma once
#include <vector>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "dx12\DX12DescriptorHeap.h"
#include "ISurfacePolicy.h"

#include <comdef.h>
using Microsoft::WRL::ComPtr;


class FDX12SurfacePolicy : public ISurfacePolicy
{
public:
	FDX12SurfacePolicy(class FDX12Factory *pFactory, HWND hWnd, UINT Width, UINT Height, bool bIsWindowed, UINT FrameCount);


protected:
	FDX12DescriptorHeap m_RTVHeap;
	ComPtr<IDXGISwapChain1> m_pSwapChain;
	std::vector< ComPtr<ID3D12Resource> > m_vRenderTargets;
		

};