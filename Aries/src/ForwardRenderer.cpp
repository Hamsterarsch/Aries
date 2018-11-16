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
	m_pDeviceResources{ DeviceResources }
{
	m_LightBufferData[0].Color = { 1, 0, 0 };
	m_LightBufferData[0].OuterAngle = 90;
	m_LightBufferData[0].InnerAngle = 80;
	m_LightBufferData[0].Range = 3.25;
	m_LightBufferData[0].ToWorldMatrix = DirectX::XMMatrixTranslation(0, .75, 0) * DirectX::XMMatrixRotationAxis({ 0,1,0 }, DirectX::XMConvertToRadians(-90));
	m_LightBufferData[0].Intensity = 4;

	auto d = ARRAYSIZE(m_LightBufferData) * sizeof(FLight);
	CD3D11_BUFFER_DESC LbDesc(d, D3D11_BIND_CONSTANT_BUFFER);
	

	D3D11_SUBRESOURCE_DATA LbResc{};
	LbResc.pSysMem = m_LightBufferData;
	

	auto res = m_pDeviceResources->GetDevice()->CreateBuffer(&LbDesc, &LbResc, &m_pLightBuffer);

	m_pDeviceResources->GetDeviceContext()->PSSetConstantBuffers(0, 1, m_pLightBuffer.GetAddressOf());


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
		
	m_DepthPrePass.m_pShaderCluster = std::make_shared<FShaderCluster>();
	m_DepthPrePass.m_pShaderCluster->m_pVertex = std::move(Vertex);
	m_DepthPrePass.m_pShaderCluster->m_pPixel = std::move(Pixel);
	
	FSingleShadowMap ShadowMapLightBuffer;
	ShadowMapLightBuffer.View = DirectX::XMMatrixLookAtRH({ 3, .75, 0 }, { 0,.75,0 }, { 0,1,0 });
	ShadowMapLightBuffer.Projection = DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(120), m_pDeviceResources->GetAspectRatio(), .01f, 100.f);

	if (!m_SingleLightView.Initialize(*m_pDeviceResources->GetDevice(), &ShadowMapLightBuffer))
	{
		throw(-1);

	}


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

	pContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	//b0 always is the world/view/projection buffer (todo: could only be called once)
	pContext->VSSetConstantBuffers(1, 1, m_SingleLightView.GetAddressOf());
	
	//DepthPrepass	
	auto test = m_pDeviceResources->GetDepthPrePassBuffer();
	auto testtest = test->GetViewTarget();
	pContext->OMSetRenderTargets(1, testtest, pDepthStencil);
	float DepthClearColor[4] = { 1,1,1,1 };
	pContext->ClearRenderTargetView(m_pDeviceResources->GetDepthPrePassBuffer()->GetViewTargetCom().Get(), DepthClearColor);
	
	vActorSet.at(0)->Rotate(0.01, 0.01, 0.02);

	m_DepthPrePass.Bind(*pContext);

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

	m_DepthPrePass.ReceiveOnFinish(*pContext);
	ClearRenderTargets();

	//For every actor
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

		/*
		if (Actor->PrepareForRender(*pContext))
		{

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
		*/

	}

	
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
