#include "VertexSingleLightShadow.h"

FVertexSingleLightShadow::FVertexSingleLightShadow(ID3D11Device *pDevice, const UINT ShadowMapWidth, const UINT ShadowMapHeight) :
	FVertexBase{ pDevice, FModulePath::MakeExeRelative(L"../../shaders/SingleLightShadow_VS.hlsl").data() }
{


	m_LightBufferData.m_View = DirectX::XMMatrixLookAtRH({ 2,2,2 }, { 0,0,0 }, { 0,1,0 });
	m_LightBufferData.m_Projection = DirectX::XMMatrixPerspectiveRH(ShadowMapWidth, ShadowMapHeight, 1, 100);

	CD3D11_BUFFER_DESC BufferDesc{ sizeof(FLightbufferData), D3D11_BIND_CONSTANT_BUFFER };

	D3D11_SUBRESOURCE_DATA BufferData{};
	BufferData.pSysMem = &m_LightBufferData;

	pDevice->CreateBuffer(&BufferDesc, &BufferData, m_pLightbuffer.GetAddressOf());


}

/*
bool FVertexSingleLightShadow::BindVertex(ID3D11DeviceContext &Context)
{
	Context.VSSetConstantBuffers(1, 1, m_pLightbuffer.GetAddressOf());

	return true;


}
*/