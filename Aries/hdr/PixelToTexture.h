#pragma once
#include "PixelBase.h"
#include "TextureTarget2D.h"

class FPixelToTexture : public FPixelBase
{
public:
	FPixelToTexture
	(
		ID3D11Device *pDevice, 
		LPCWSTR pPath, 
		LPCSTR pEntry, 
		float TextureWidth, 
		float TextureHeight, 
		DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32A32_FLOAT
	);

private:
	virtual bool BindPixel(ID3D11DeviceContext &Context) override;

	FTextureTarget2D m_TextureTarget;

	
};

