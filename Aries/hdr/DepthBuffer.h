#pragma once
#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class FDepthBuffer
{
public:
	FDepthBuffer() = default;
	FDepthBuffer(ID3D11Device *pDevice, UINT Width, UINT Height, DXGI_FORMAT Format);

	bool Resize(ID3D11Device *pDevice, UINT Width, UINT Height);
	

	ComPtr<ID3D11Texture2D> m_pTexture;
	ComPtr<ID3D11DepthStencilView> m_pViewDepth;
	ComPtr<ID3D11ShaderResourceView> m_pViewResource;


private:
	DXGI_FORMAT m_TextureFormat;


};

