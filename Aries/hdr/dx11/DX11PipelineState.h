#pragma once
#include <d3d11.h>
#include "IPipelineState.h"



class FDX11PipelineState : public IPipelineState
{
public:
	FDX11PipelineState() = default;
	FDX11PipelineState(const ComPtr<ID3D11DeviceContext> &pContext);

	//virtual void BindBlendState(uint Slot, FBlendState &&mov_BlendState) override;
	//virtual void BindDepthStencilState(uint Slot, FDepthStencilState &&mov_DepthStencilState) override;



protected:	
	ComPtr<ID3D11DeviceContext> m_pContext;


};


