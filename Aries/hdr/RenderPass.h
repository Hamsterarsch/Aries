#pragma once
#include <D3D11.h>
#include <functional>
#include "ShaderCluster.h"
#include "IContextBinding.h"
#include "PassMetadata.h"

class FRenderPass : public IContextBinding
{
public:
	void ReceiveOnBegin(ID3D11DeviceContext &Context)
	{
		if (m_fOnBegin)
		{
			m_fOnBegin(Context, m_pPassMetadata);

		}


	}

	void ReceiveOnFinish(ID3D11DeviceContext &Context)
	{
		if (m_fOnFinish)
		{
			m_fOnFinish(Context, m_pPassMetadata);

		}


	}

	virtual bool Bind(ID3D11DeviceContext &Context) override
	{
		return m_pShaderCluster ? m_pShaderCluster->Bind(Context) : false;
		

	}

	std::function<void(ID3D11DeviceContext &Context, std::shared_ptr<FPassMetadata> &)> m_fOnBegin;
	std::function<void(ID3D11DeviceContext &Context, std::shared_ptr<FPassMetadata> &)> m_fOnFinish;
	std::shared_ptr<FShaderCluster> m_pShaderCluster;
	std::shared_ptr<FPassMetadata> m_pPassMetadata;


};
