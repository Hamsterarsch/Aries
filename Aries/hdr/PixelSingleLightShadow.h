#pragma once
#include "PixelBase.h"
#include "TextureTarget2D.h"

class FPixelSingleLightShadow final : public FPixelBase
{
public:
	FPixelSingleLightShadow(ID3D11Device *pDevice, UINT ShadowMapWidth, UINT ShadowMapHeight);
	
private:
	ComPtr<ID3D11Texture2D> m_pTexture;
	ComPtr<ID3D11DepthStencilView> m_pViewDepth;
	ComPtr<ID3D11ShaderResourceView> m_pViewResource;

	ComPtr<ID3D11SamplerState> m_pSamplerStateClamp;



};