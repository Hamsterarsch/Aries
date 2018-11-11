#include "PixelToTexture.h"

FPixelToTexture::FPixelToTexture
(	
	ID3D11Device *pDevice,
	LPCWSTR pPath,
	LPCSTR pEntry,
	float TextureWidth,
	float TextureHeight,
	DXGI_FORMAT Format
)	:
	m_TextureTarget{ *pDevice, TextureWidth, TextureHeight, Format },
	FPixelBase{ pDevice, pPath, pEntry }
{}

bool FPixelToTexture::BindPixel(ID3D11DeviceContext &Context)
{
	Context.OMSetRenderTargets(1, m_TextureTarget.GetViewTarget(), nullptr);
	
	return true;


}