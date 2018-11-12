#include "DepthBuffer.h"
#include "Error.h"

FDepthBuffer::FDepthBuffer(ID3D11Device *pDevice, const UINT Width, const UINT Height, const DXGI_FORMAT Format)
{
	D3D11_TEXTURE2D_DESC Desc;
	Desc.Format = Format;
	Desc.Width = Width;
	Desc.Height = Height;
	Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.SampleDesc.Count = 1;
	Desc.Usage = D3D11_USAGE_DEFAULT;
		
	auto Hr = pDevice->CreateTexture2D(&Desc, nullptr, m_pTexture.GetAddressOf());
	if (FAILED(Hr))
	{
		throw(FError{ Hr, "Could not create texture for depth buffer.", __FILE__, __LINE__ });
		
	}


	D3D11_DEPTH_STENCIL_VIEW_DESC ViewDepthDesc;
	ViewDepthDesc.Format = Desc.Format;
	ViewDepthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	
	Hr = pDevice->CreateDepthStencilView(m_pTexture.Get(), &ViewDepthDesc, m_pViewDepth.GetAddressOf());
	if (FAILED(Hr))
	{
		throw(FError{ Hr, "Could not create depth view for depth buffer.", __FILE__, __LINE__ });

	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ViewResourceDesc{};
	ViewResourceDesc.Format = Desc.Format;
	ViewResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ViewResourceDesc.Texture2D.MipLevels = 1;	

	Hr = pDevice->CreateShaderResourceView(m_pTexture.Get(), &ViewResourceDesc, m_pViewResource.GetAddressOf());
	if (FAILED(Hr))
	{
		throw(FError{ Hr, "Could not create resource view for depth buffer.", __FILE__, __LINE__ });

	}


}

bool FDepthBuffer::Resize(ID3D11Device *pDevice, const UINT Width, const UINT Height)
{
	bool bFullySuccessful{ true };

	D3D11_TEXTURE2D_DESC Desc{};
	m_pTexture->GetDesc(&Desc);
	Desc.Width = Width;
	Desc.Height = Height;
	auto Hr = pDevice->CreateTexture2D(&Desc, nullptr, m_pTexture.GetAddressOf());
	bFullySuccessful &= (Hr == S_OK);

	D3D11_DEPTH_STENCIL_VIEW_DESC ViewDepthDesc{};
	m_pViewDepth->GetDesc(&ViewDepthDesc);
	Hr = pDevice->CreateDepthStencilView(m_pTexture.Get(), &ViewDepthDesc, m_pViewDepth.GetAddressOf());
	bFullySuccessful &= (Hr == S_OK);

	D3D11_SHADER_RESOURCE_VIEW_DESC ViewResourceDesc{};
	m_pViewResource->GetDesc(&ViewResourceDesc);
	Hr = pDevice->CreateShaderResourceView(m_pTexture.Get(), &ViewResourceDesc, m_pViewResource.GetAddressOf());
	bFullySuccessful &= (Hr == S_OK);

	return bFullySuccessful;


}