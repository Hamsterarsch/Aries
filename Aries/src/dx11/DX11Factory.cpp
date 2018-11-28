#include "Error.h"
#include "dx11/DX11Factory.h"


//Public

/*
FDX11Factory &FDX11Factory::GetInstance()
{
	if (!s_pInstance)
	{
		s_pInstance = std::unique_ptr<FDX11Factory>(new FDX11Factory());

	}

	return *s_pInstance.get();


}

IPipelineState &FDX11Factory::GetPipelineState() noexcept
{
	return m_PipelineState;

}

FDX11Factory::FDX11Factory()
{
	D3D_FEATURE_LEVEL TargetFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1

	};

	UINT DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	auto Hr = D3D11CreateDevice
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		DeviceFlags,
		TargetFeatureLevels,
		ARRAYSIZE(TargetFeatureLevels),
		D3D11_SDK_VERSION,
		m_pDevice.GetAddressOf(),
		&m_DeviceLevel,
		m_pDeviceContext.GetAddressOf()
	);

	ARI_ASSERT(SUCCEEDED(Hr), "Could not create d3d11 device.");
	
	m_PipelineState = FDX11PipelineState(m_pDeviceContext);


}*/