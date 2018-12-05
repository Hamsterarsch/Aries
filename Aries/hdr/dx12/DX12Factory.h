#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <memory>
#include "IGAPIFactoryInternal.h"
#include "IWindow.h"
#include "ISurfacePolicy.h"

using Microsoft::WRL::ComPtr;

class FDX12Factory : public IGAPIFactoryInternal
{
public:
	static std::shared_ptr<IGAPIFactory> GetInstance();

	IDXGIFactory4 *GetDXGIFactory() const noexcept { return m_pDXGIFactory.Get(); }
	ID3D12Device *GetDevice() const noexcept { return m_pDevice.Get(); }
	ID3D12CommandQueue *GetFactoryCmdQ() const noexcept { return m_pFactoryCmdQ.Get(); }

	virtual EAPIType GetApiType() const override { return EAPIType::DX12; }
	virtual std::unique_ptr<IWindow> MakeWindow(UINT Width, UINT Height, LPCWSTR pWindowName, WNDPROC pfnWndProc, LPCWSTR pClassName) override;


protected:
	FDX12Factory();


	static std::shared_ptr<FDX12Factory> s_pInstance;


	bool m_bUseWarpDevice;
	ComPtr<ID3D12Device> m_pDevice;
	ComPtr<IDXGIFactory4> m_pDXGIFactory;
	ComPtr<ID3D12CommandQueue> m_pFactoryCmdQ;
	ComPtr<ID3D12CommandAllocator> m_pCommonCmdQAlloc;
	
};
