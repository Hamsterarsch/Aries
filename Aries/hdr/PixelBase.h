#pragma once
#include "ShaderBase.h"
#include <functional>

class FPixelBase : public FShaderBase<ID3D11PixelShader>
{
public:
	FPixelBase(ID3D11Device *pDevice, LPCWSTR pPath, LPCSTR pEntry = "main", std::function<bool(ID3D11DeviceContext &)> BindFunction = nullptr) :
		m_BindFunction{ std::move(BindFunction) },
		FShaderBase<ID3D11PixelShader>{ pDevice, pEntry, pPath, "ps_5_0", &ID3D11Device::CreatePixelShader }
	{		
	}

	virtual bool Bind(ID3D11DeviceContext &Context) final override;
	

private:
	virtual bool BindPixel(ID3D11DeviceContext &Context) { return true; }

	std::function<bool(ID3D11DeviceContext &)> m_BindFunction;


};