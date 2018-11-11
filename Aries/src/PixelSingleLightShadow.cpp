#include "PixelSingleLightShadow.h"

FPixelSingleLightShadow::FPixelSingleLightShadow(ID3D11Device *pDevice, UINT ShadowmapWidth, UINT ShadowmapHeight) :
	FPixelBase{ pDevice, FModulePath::MakeExeRelative(L"../../shaders/SingleLightShadow_PS.hlsl").data() }
{
	float aClearColor[4] = { 0,0,0,0 };

	CD3D11_SAMPLER_DESC SamplerDesc
	{
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		0,
		1,
		D3D11_COMPARISON_ALWAYS,
		aClearColor,
		0,
		D3D11_FLOAT32_MAX

	};
	pDevice->CreateSamplerState(&SamplerDesc, m_pSamplerStateClamp.GetAddressOf());


	CD3D11_TEXTURE2D_DESC TextureDesc
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		ShadowmapWidth,
		ShadowmapHeight
	};
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	pDevice->CreateTexture2D(&TextureDesc, nullptr, m_pTexture.GetAddressOf());
	/*
	CD3D11_DEPTH_STENCIL_VIEW_DESC DsDesc
	{
	}
	*/

}

bool FPixelSingleLightShadow::BindPixel(ID3D11DeviceContext &Context)
{
	Context.PSSetSamplers(0, 1, m_pSamplerStateClamp.GetAddressOf());
		
	return true;


}