#pragma once
#include "ShaderBase.h"
#include <functional>

class FVertexBase : public FShaderBase<ID3D11VertexShader>
{
public:
	FVertexBase(ID3D11Device *pDevice, LPCWSTR pPath, LPCSTR pEntry = "main") :
		FShaderBase<ID3D11VertexShader>{ pDevice, pEntry, pPath, "vs_5_0", &ID3D11Device::CreateVertexShader }
	{
	}

	virtual bool Bind(ID3D11DeviceContext &Context) final override;


};
