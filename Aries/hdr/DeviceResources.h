#pragma once
#include <D3D11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


class FDeviceResources
{
public:
	FDeviceResources(HWND hWnd);
	~FDeviceResources();

	FDeviceResources(FDeviceResources &&Other) = default;
	FDeviceResources &operator=(FDeviceResources &&Rhs) = default;

	void PresentBackbuffer();
	void GoWindowed(UINT Width, UINT Height);
	void GoFullscreen();
	float GetAspectRatio() const { return m_BackbufferDesc.Width / static_cast<float>(m_BackbufferDesc.Height); }
	ID3D11Device*			GetDevice() const { return m_pDevice.Get(); }
	ID3D11DeviceContext*	GetDeviceContext() const { return m_pDeviceContext.Get(); }
	ID3D11RenderTargetView* GetRenderTarget() const { return m_pBackBufferView.Get(); }
	ID3D11DepthStencilView* GetDepthStencil() const { return m_pDepthStencilView.Get(); }
	D3D_FEATURE_LEVEL		GetFeatureLevel() const { return m_DeviceFeatureLevel; }

private:
	void AcquireDevice(ComPtr<ID3D11Device> &out_pDevice, ComPtr<ID3D11DeviceContext> &out_pContext);

	void AcquireWindowResources(ComPtr<IDXGISwapChain> &out_pSwapChain, const ComPtr<ID3D11Device> &Device, HWND hWnd);

	void AcquireBackbuffer
	(
		ComPtr<ID3D11Texture2D> &out_pBackbuffer,
		ComPtr<ID3D11RenderTargetView> &out_pBackbufferView,
		D3D11_TEXTURE2D_DESC &out_BackbufferDesc, 
		const ComPtr<IDXGISwapChain> &pSwapChain, 
		const ComPtr<ID3D11Device> &pDevice
	);

	void AcquireDepthStencil
	(
		ComPtr<ID3D11Texture2D> &out_pDepthStencil,
		ComPtr<ID3D11DepthStencilView> &out_pDepthStencilView,
		const D3D11_TEXTURE2D_DESC &BackbufferDesc, 
		const ComPtr<IDXGISwapChain> &pSwapChain, 
		const ComPtr<ID3D11Device> &pDevice
	);

	void FitViewportSize(ComPtr<ID3D11DeviceContext> &out_pDeviceContext, D3D11_VIEWPORT &out_Viewport, const D3D11_TEXTURE2D_DESC &BackbufferDesc);

	void ReleaseBackbuffer();

	//DeviceResc
	ComPtr<ID3D11Device> m_pDevice;			
	ComPtr<ID3D11DeviceContext>	m_pDeviceContext;
	ComPtr<IDXGISwapChain> m_pSwapChain;

	//Backbuffer
	ComPtr<ID3D11Texture2D> m_pBackBuffer;
	ComPtr<ID3D11RenderTargetView> m_pBackBufferView;

	//DepthStencil
	ComPtr<ID3D11Texture2D>	m_pDepthStencil;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	//Metadata
	D3D_FEATURE_LEVEL m_DeviceFeatureLevel;			
	D3D11_TEXTURE2D_DESC m_BackbufferDesc;		
	D3D11_VIEWPORT m_Viewport;						


};
