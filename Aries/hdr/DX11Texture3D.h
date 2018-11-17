#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Error.h"

using Microsoft::WRL::ComPtr;

class FDX11Texture3D
{
public:
	FDX11Texture3D(ID3D11Device &Device, UINT Width, UINT Height, UINT Num, DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32A32_FLOAT)
	{
		D3D11_TEXTURE3D_DESC Desc{};
		Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		Desc.Height = Height;
		Desc.Width = Width;
		Desc.Depth = Num;
		Desc.Format = Format;
		Desc.MipLevels = 1;
		Desc.Usage = D3D11_USAGE_DEFAULT;

		auto Hr = Device.CreateTexture3D(&Desc, nullptr, m_pTexture3D.GetAddressOf());
		if (FAILED(Hr))
		{
			throw(FError{ Hr, "Could not create texture 3D", __FILE__, __LINE__ });


		}

		D3D11_RENDER_TARGET_VIEW_DESC ViewTargetDesc{};
		ViewTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		ViewTargetDesc.Format = Format;
		
		Hr = Device.CreateRenderTargetView(m_pTexture3D.Get(), &ViewTargetDesc, m_pViewTarget.GetAddressOf());
		if (FAILED(Hr))
		{
			throw(FError{ Hr, "Could not create render target view for texture 3D", __FILE__, __LINE__ });


		}
		
		D3D11_SHADER_RESOURCE_VIEW_DESC ViewResourceDesc{};
		ViewResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		ViewResourceDesc.Format = Format;
		
		Hr = Device.CreateShaderResourceView(m_pTexture3D.Get(), &ViewResourceDesc, m_pViewResource.GetAddressOf());
		if (FAILED(Hr))
		{
			throw(FError{ Hr, "Could not create resource view texture 3D", __FILE__, __LINE__ });


		}

	}

	DXGI_FORMAT GetFormat() const noexcept { return m_Format; }
	UINT GetHeight() const noexcept { return m_Height; }
	UINT GetWidth() const noexcept { return m_Width; }
	ID3D11RenderTargetView *const *GetViewTarget() const noexcept { return m_pViewTarget.GetAddressOf();  }
	ID3D11ShaderResourceView *const *GetViewResource() const noexcept { return m_pViewResource.GetAddressOf(); }

protected:

	DXGI_FORMAT m_Format;
	UINT	m_Width, 
			m_Height;
	ComPtr<ID3D11Texture3D> m_pTexture3D;
	ComPtr<ID3D11RenderTargetView> m_pViewTarget;
	ComPtr<ID3D11ShaderResourceView> m_pViewResource;


};
