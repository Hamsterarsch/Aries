#include "TextureTarget2D.h"

//Public----------------

FTextureTarget2D::FTextureTarget2D(ID3D11Device &Device, const float TextureWidth, const float TextureHeight, DXGI_FORMAT Format)
{
	D3D11_TEXTURE2D_DESC TextureDesc{};
	TextureDesc.Width = TextureWidth;
	TextureDesc.Height = TextureHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = Format;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	auto Hr = Device.CreateTexture2D(&TextureDesc, NULL, m_pTexture.GetAddressOf());
	//err check

	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceViewDesc{};
	ResourceViewDesc.Format = TextureDesc.Format;
	ResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ResourceViewDesc.Texture2D.MipLevels = TextureDesc.MipLevels;

	Hr = Device.CreateShaderResourceView(m_pTexture.Get(), &ResourceViewDesc, m_pResourceView.GetAddressOf());
	//err check

	D3D11_RENDER_TARGET_VIEW_DESC RenderViewDesc{};
	RenderViewDesc.Format = TextureDesc.Format;
	RenderViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	Hr = Device.CreateRenderTargetView(m_pTexture.Get(), &RenderViewDesc, m_pRenderView.GetAddressOf());
	//err check


}


//Private--------------------

ID3D11ShaderResourceView *const *FTextureTarget2D::GetViewResource() const noexcept
{
	return m_pResourceView.GetAddressOf();


}

ID3D11RenderTargetView *const *FTextureTarget2D::GetViewTarget() const noexcept
{
	return m_pRenderView.GetAddressOf();


}

void FTextureTarget2D::Clear(ID3D11DeviceContext &Context)
{
	Context.ClearRenderTargetView(m_pRenderView.Get(), s_ClearColor);


}