#pragma once
#include "VertexBase.h"
#include <DirectXMath.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class FVertexSingleLightShadow final : public FVertexBase
{
public:
	FVertexSingleLightShadow(ID3D11Device *pDevice, UINT ShadowMapWidth, UINT ShadowMapHeight);


private:
	struct FLightbufferData
	{
		DirectX::XMMATRIX	m_View,
							m_Projection;

	} m_LightBufferData;
		
	ComPtr<ID3D11Buffer> m_pLightbuffer;

	
};