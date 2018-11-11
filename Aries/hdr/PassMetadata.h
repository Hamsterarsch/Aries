#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

using Microsoft::WRL::ComPtr;

struct FPassMetadata
{
	std::vector< ComPtr<ID3D11ShaderResourceView>> m_vResourceViews;
	std::vector< ComPtr<ID3D11SamplerState>> m_vSamplerStates;
	std::vector< ComPtr<ID3D11RenderTargetView>> m_vRenderTargets;
	std::vector< ComPtr<ID3D11Buffer>> m_vBuffers;


};
