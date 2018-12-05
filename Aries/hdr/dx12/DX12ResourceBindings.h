#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "IResourceBindings.h"

using Microsoft::WRL::ComPtr;

class FDX12ResourceBindings : public IResourceBindings
{
public:
	FDX12ResourceBindings();

protected:
	ComPtr<ID3D12RootSignature> m_pRootSignature;


};
