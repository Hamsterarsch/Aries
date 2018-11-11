#pragma once
#include <D3D11.h>
#include <functional>
#include "ShaderCluster.h"
#include "IContextBinding.h"

class FRenderPass : public IContextBinding
{
public:
	void ReceiveOnBegin()
	{
		if (m_fOnBegin)
		{
			m_fOnBegin();

		}


	}

	void ReceiveOnFinish()
	{
		if (m_fOnFinish)
		{
			m_fOnFinish();

		}


	}

	virtual bool Bind(ID3D11DeviceContext &Context) override
	{
		return m_pShaderCluster ? m_pShaderCluster->Bind(Context) : false;
		

	}

	std::function<void()> m_fOnBegin;
	std::function<void()> m_fOnFinish;
	std::shared_ptr<FShaderCluster> m_pShaderCluster;


};
