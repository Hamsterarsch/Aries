#include "ForwardRenderer.h"
#include "DeviceResources.h"
#include "Error.h"
#include "ModulePath.h"
#include "IMesh.h"
#include "VertexBase.h"
#include "PixelBase.h"
#include <string>


template<class T>
void InvertWindingOrder(std::vector<T> &out_ToSwap)
{
	for (size_t Index{}; Index < out_ToSwap.size(); Index += 3)
	{
		auto Elem1 = out_ToSwap.at(Index);
		out_ToSwap.at(Index) = out_ToSwap.at(Index + 2);
		out_ToSwap.at(Index + 2) = Elem1;
		

	}


}

FForwardRenderer::FForwardRenderer(std::shared_ptr<FDeviceResources> DeviceResources) :
	m_pDeviceResources{ DeviceResources },
	m_ShadowMapPool512{ *DeviceResources->GetDevice(), 512, 512, MAX_LIGHTMAPS },
	m_ShadowmapDepthBuffer{ DeviceResources->GetDevice(), 512, 512, DXGI_FORMAT_D32_FLOAT }
{
	auto ByteWidth = sizeof(decltype(m_vLightBufferData)::value_type);
	CD3D11_BUFFER_DESC Desc{ ByteWidth, D3D11_BIND_CONSTANT_BUFFER };

	auto Hr = m_pDeviceResources->GetDevice()->CreateBuffer(&Desc, nullptr, m_pLightBuffer.GetAddressOf());
	if (FAILED(Hr))
	{
		throw(FError{ Hr, "Could not create light buffer", __FILE__, __LINE__ });

	}

	
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	m_pDeviceResources->GetDeviceContext()->UpdateSubresource
	(
		m_pConstantBuffer.Get(),
		0,
		nullptr,
		&m_ConstantBufferData,
		0,
		0
	);
	
	
	auto Vertex = std::make_shared<FVertexBase>( m_pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/DepthToTexture_VS.hlsl").data() );
	auto Pixel = std::make_shared<FPixelBase>( m_pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/DepthToTexture_PS.hlsl").data() );
		
	m_LightmapPass.m_pShaderCluster = std::make_shared<FShaderCluster>();
	m_LightmapPass.m_pShaderCluster->m_pVertex = std::move(Vertex);
	m_LightmapPass.m_pShaderCluster->m_pPixel = std::move(Pixel);
	

}

void FForwardRenderer::ClearRenderTargets()
{
	static float ClearColor[] = { .5f, .5f, .5f, 1.f };
	
	m_pDeviceResources->GetDeviceContext()->ClearRenderTargetView(m_pDeviceResources->GetRenderTarget(), ClearColor);
	m_pDeviceResources->GetDeviceContext()->ClearDepthStencilView
	(
		m_pDeviceResources->GetDepthStencil(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.f,
		0
	);


}

void FForwardRenderer::Render(const  std::vector<std::unique_ptr<IActor>> &vActorSet)
{
	auto WorldMatrixBase =	DirectX::XMMatrixRotationY( DirectX::XMConvertToRadians(static_cast<float>(++m_FrameCount)) )
							* DirectX::XMMatrixRotationX( DirectX::XMConvertToRadians(static_cast<float>(m_FrameCount)) )
							* DirectX::XMMatrixIdentity();
								

	if (m_FrameCount == MAXUINT)
	{
		m_FrameCount = 0;

	}

	ID3D11DeviceContext *pContext = m_pDeviceResources->GetDeviceContext();
	ID3D11RenderTargetView *pRenderTarget = m_pDeviceResources->GetRenderTarget();
	ID3D11DepthStencilView *pDepthStencil = m_pDeviceResources->GetDepthStencil();

	//Clear target each frame
	ID3D11RenderTargetView * Null = nullptr;
	pContext->OMSetRenderTargets(1, &Null, nullptr);
	ClearRenderTargets();

	//b0 always is the world/view/projection buffer (todo: could only be called once)
	pContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());


	//LightmapPass
	m_LightmapPass.Bind(*pContext);
	m_LightmapPass.ReceiveOnBegin(*pContext);
	
	float DepthClearColor[4] = { 1,1,1,1 };
	pContext->ClearDepthStencilView(m_ShadowmapDepthBuffer.m_pViewDepth.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	pContext->ClearRenderTargetView(*m_ShadowMapPool512.GetViewTarget(), DepthClearColor);

	pContext->OMSetRenderTargets(1, m_ShadowMapPool512.GetViewTarget(), m_ShadowmapDepthBuffer.m_pViewDepth.Get());
	pContext->VSSetConstantBuffers(1, 1, m_pLightBuffer.GetAddressOf());


	for (auto &&Light : m_vLightBufferData)
	{
		m_pDeviceResources->GetDeviceContext()->UpdateSubresource(m_pLightBuffer.Get(), 0, nullptr, &Light, 0, 0);

		for (auto &&Actor : vActorSet)
		{
			if (!Actor->GetMesh()->Bind(*pContext))
			{
				continue;

			}

			m_ConstantBufferData.World = Actor->GetToWorldMatrix();// *WorldMatrixBase;
			pContext->UpdateSubresource
			(
				m_pConstantBuffer.Get(),
				0,
				nullptr,
				&m_ConstantBufferData,
				0,
				0
			);

			pContext->DrawIndexed
			(
				static_cast<UINT>(Actor->GetMesh()->GetIbSize()),
				0,
				0

			);


		}
	
	}
	
	m_LightmapPass.ReceiveOnFinish(*pContext);
	//ClearRenderTargets();

	//For every actor
	/*DISABLED FOR LIGHTMAP DEBUG
	for (auto &&Actor : vActorSet)
	{
		if (!Actor->GetMesh()->Bind(*pContext))
		{
			continue;

		}

		//Set RT's and Update output merger https://msdn.microsoft.com/de-de/library/windows/desktop/bb205120(v=vs.85).aspx
		pContext->OMSetRenderTargets
		(
			1,
			&pRenderTarget,
			pDepthStencil

		);

		m_ConstantBufferData.World = Actor->GetToWorldMatrix();// *WorldMatrixBase;
		pContext->UpdateSubresource
		(
			m_pConstantBuffer.Get(),
			0,
			nullptr,
			&m_ConstantBufferData,
			0,
			0
		);	
		
		//For every pass
		for (auto &&Pass : Actor->GetRenderPasses())
		{
			Pass->ReceiveOnBegin(*pContext);
			if (!Pass->Bind(*pContext))
			{
				break;

			}

			pContext->DrawIndexed
			(
				static_cast<UINT>(Actor->GetMesh()->GetIbSize()),
				0,
				0
			
			);
			Pass->ReceiveOnFinish(*pContext);


		}


	}
	*/

	
}

void FForwardRenderer::CreateWindowSizeDependentResources()
{
	CreateViewAndPerspective();

}

void FForwardRenderer::CreateViewAndPerspective()
{
	DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, .5, -2.f, 0.f);
	DirectX::XMVECTOR At =  DirectX::XMVectorSet(0.0f, .5, 0.0f, 0.f);
	DirectX::XMVECTOR Up =  DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

	//DirectXMath uses row major matrices, we want column major
	m_ConstantBufferData.View = DirectX::XMMatrixLookAtRH(Eye, At, Up);

	m_ConstantBufferData.Projection = DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(70.f), m_pDeviceResources->GetAspectRatio(), .01f, 100.f);
	
	//m_ConstantBufferData.World = DirectX::XMMatrixIdentity();


}

void FForwardRenderer::CreateDeviceDependentResources()
{
	CD3D11_BUFFER_DESC CbDesc(sizeof(ConstantBufferStruct), D3D11_BIND_CONSTANT_BUFFER);

	auto Hr = m_pDeviceResources->GetDevice()->CreateBuffer
	(
		&CbDesc,
		nullptr,
		&m_pConstantBuffer
	);
	if (FAILED(Hr))
	{
		throw(FError{ Hr, "Failed to create constant buffer", __FILE__, __LINE__ });

	}


}
