#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "IGAPIFactory.h"
#include "dx11/DX11PipelineState.h"

using Microsoft::WRL::ComPtr;

/*
class FDX11Factory :public IGAPIFactory
{
public:
	static FDX11Factory &GetInstance();

	virtual IPipelineState &GetPipelineState() noexcept;


protected:
	FDX11Factory();


	static std::unique_ptr<FDX11Factory> s_pInstance;


	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	D3D_FEATURE_LEVEL m_DeviceLevel;
	FDX11PipelineState m_PipelineState;

};
*/