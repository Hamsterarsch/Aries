#pragma once
#include "ShaderBase.h"


struct FShaderCluster
{
public:
	bool Bind(ID3D11DeviceContext &Context)
	{
		bool bBindSuccessful{ true };
		if (m_pVertex)
		{
			bBindSuccessful &= m_pVertex->Bind(Context);
		
		}
		else
		{
			return false;

		}

		if (m_pPixel)
		{
			bBindSuccessful &= m_pPixel->Bind(Context);
		
		}
		else
		{
			return false;

		}

		return bBindSuccessful;


	}

	std::shared_ptr<FShaderBase<ID3D11VertexShader>> m_pVertex;
	std::shared_ptr<FShaderBase<ID3D11PixelShader>> m_pPixel;


};
