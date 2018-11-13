#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "IContextBinding.h"

using Microsoft::WRL::ComPtr;

class FTextureTarget2D final
{
public:
	FTextureTarget2D() = default;
	FTextureTarget2D(ID3D11Device &Device, float TextureWidth, float TextureHeight, DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32A32_FLOAT);

	ID3D11ShaderResourceView *const *GetViewResource() const noexcept;
	ID3D11RenderTargetView *const *GetViewTarget() const noexcept;

	ComPtr<ID3D11ShaderResourceView> GetViewResourceCom() 
	{
		return m_pResourceView;


	}

	ComPtr<ID3D11RenderTargetView> GetViewTargetCom()
	{
		return m_pRenderView;


	}

	void Clear(ID3D11DeviceContext &Context);


private:
	static constexpr float s_ClearColor[4] = { 1,1,1,1 };
	
	ComPtr<ID3D11Texture2D> m_pTexture;
	ComPtr<ID3D11ShaderResourceView> m_pResourceView;
	ComPtr<ID3D11RenderTargetView> m_pRenderView;

	

};
