#pragma once
#include <wrl/client.h>
#include <D3D11.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "PixelBase.h"
#include "VertexBase.h"
#include "IActor.h"
#include "Light.h"
#include "RenderPass.h"
#include "hdr/dx11/DX11ConstantBuffer.h"
#include "DX11Texture3D.h"
#include <algorithm>
#include "DepthBuffer.h"


using Microsoft::WRL::ComPtr;


class FForwardRenderer
{
public:
	FForwardRenderer(std::shared_ptr<class FDeviceResources> DeviceResources);
	void Render(const std::vector<std::unique_ptr<IActor>> &vActorSet);
	DirectX::XMMATRIX GetReflectMatrix() { return DirectX::XMMatrixScaling(1, -1, 1) * m_ConstantBufferData.View; }
	DirectX::XMMATRIX GetViewMatrix() { return m_ConstantBufferData.View; }

	static constexpr UINT MAX_LIGHTMAPS = 30;
	
	bool AddLight(FLight &&mov_LightInfo, UINT Resolution)
	{
		if (Resolution == 512)
		{
			mov_LightInfo.LightMapStartIndex = m_FreeIndex;
			mov_LightInfo.LightMapStartChannel = m_FreeChannel;
			mov_LightInfo.ProjectionMatrix = DirectX::XMMatrixOrthographicRH(Resolution, Resolution, .01, mov_LightInfo.Range);
			if (mov_LightInfo.OuterAngle >= 359)
			{
				auto NewFreeChannel = (m_FreeChannel + 6) % 4;
				auto IndexOverflow = std::floor((m_FreeChannel + 6) / 4);
				
				m_FreeChannel = NewFreeChannel;
				m_FreeIndex += IndexOverflow;
				
				if (m_FreeIndex >= MAX_LIGHTMAPS)
				{
					return false;

				}


			}
			else
			{
				auto NewFreeChannel = (m_FreeChannel + 1) % 4;
				auto IndexOverflow = std::floor((m_FreeChannel + 1) / 4);

				m_FreeChannel = NewFreeChannel;
				m_FreeIndex += IndexOverflow;

				if (m_FreeChannel >= MAX_LIGHTMAPS)
				{
					return false;

				}

			}
			m_vLightBufferData.push_back(std::move(mov_LightInfo));

		}
		else
		{
			return true;

		}


	}
	
	/*
	void UpdateLightBuffer()
	{
		if (m_pLightBuffer || m_LightBufferElements != m_vLightBufferData.size())
		{
			m_LightBufferElements = static_cast<UINT>(m_vLightBufferData.size());
			auto ByteWidth = static_cast<UINT>(m_vLightBufferData.size() * sizeof(decltype(m_vLightBufferData)::value_type));
			CD3D11_BUFFER_DESC Desc{ByteWidth, D3D11_BIND_CONSTANT_BUFFER};
			
			auto Hr = m_pDeviceResources->GetDevice()->CreateBuffer(&Desc, nullptr, m_pLightBuffer.GetAddressOf());
			if (FAILED(Hr))
			{
				throw(FError{ Hr, "Could not create light buffer", __FILE__, __LINE__ });

			}

		}
		m_pDeviceResources->GetDeviceContext()->UpdateSubresource(m_pLightBuffer.Get(), 0, nullptr, m_vLightBufferData.data(), 0, 0);

	}
	*/

private:
	void CreateViewAndPerspective();
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	void ClearRenderTargets();
	
	typedef __declspec(align(16)) struct _constantBufferStruct
	{
		DirectX::XMMATRIX	World,
						    View,
						    Projection;

	} ConstantBufferStruct;
	static_assert((sizeof(ConstantBufferStruct) % 16) == 0, "Constant buffer size must be 16 byte aligned");

	std::vector<FLight> m_vLightBufferData;
	ComPtr<ID3D11Buffer> m_pLightBuffer;
	ConstantBufferStruct m_ConstantBufferData;
	ComPtr<ID3D11Buffer> m_pConstantBuffer;
	UINT m_FrameCount;
	std::shared_ptr<class FDeviceResources> m_pDeviceResources;
	FDX11Texture3D m_ShadowMapPool512;
	UINT m_FreeIndex;
	UINT m_FreeChannel;
	UINT m_LightBufferElements;

	FDepthBuffer m_ShadowmapDepthBuffer;

	FRenderPass m_LightmapPass;

};
