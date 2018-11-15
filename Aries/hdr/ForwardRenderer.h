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


using Microsoft::WRL::ComPtr;

__declspec(align(16))
struct FSingleShadowMap
{
	DirectX::XMMATRIX View,
					  Projection;

};

class FForwardRenderer
{
public:
	FForwardRenderer(std::shared_ptr<class FDeviceResources> DeviceResources);
	void Render(const std::vector<std::unique_ptr<IActor>> &vActorSet);
	DirectX::XMMATRIX GetReflectMatrix() { return DirectX::XMMatrixScaling(1, -1, 1) * m_ConstantBufferData.View; }
	DirectX::XMMATRIX GetViewMatrix() { return m_ConstantBufferData.View; }

	FDX11ConstantBuffer<FSingleShadowMap> m_SingleLightView;


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

	FLight m_LightBufferData[1];
	ComPtr<ID3D11Buffer> m_pLightBuffer;
	ConstantBufferStruct m_ConstantBufferData;
	ComPtr<ID3D11Buffer> m_pConstantBuffer;
	UINT m_FrameCount;
	std::shared_ptr<class FDeviceResources> m_pDeviceResources;

	FRenderPass m_DepthPrePass;

};
