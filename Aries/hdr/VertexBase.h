#pragma once
#include "ShaderBase.h"
#include <functional>

class FVertexBase : public FShaderBase<ID3D11VertexShader>
{
public:
	FVertexBase(ID3D11Device *pDevice, LPCWSTR pPath, LPCSTR pEntry = "main", std::function<bool(ID3D11DeviceContext &)> BindFunction = nullptr) :
		m_BindFunction{ std::move(BindFunction) },
		FShaderBase<ID3D11VertexShader>{ pDevice, pEntry, pPath, "vs_5_0", &ID3D11Device::CreateVertexShader }
	{
	}

	virtual bool Bind(ID3D11DeviceContext &Context) final override;


private:
	virtual bool BindVertex(ID3D11DeviceContext &Context) { return true; }

	std::function<bool(ID3D11DeviceContext &)> m_BindFunction;


};
