#pragma once
#include "ShaderBase.h"
#include <functional>

class FPixelBase : public FShaderBase<ID3D11PixelShader>
{
public:
	FPixelBase(ID3D11Device *pDevice, LPCWSTR pPath, LPCSTR pEntry = "main") :
		FShaderBase<ID3D11PixelShader>{ pDevice, pEntry, pPath, "ps_5_0", &ID3D11Device::CreatePixelShader }
	{		
	}

	virtual bool Bind(ID3D11DeviceContext &Context) final override;
	

};