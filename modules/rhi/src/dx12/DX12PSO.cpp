#include "dx12/DX12Factory.h"
#include "dx12/DX12PSO.h"

/*
FDX12PSO::FDX12PSO(FDX12Factory *pFactory, const FPSOInfo &Info)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc{};


#pragma region BlendDepthStencil
	Desc.BlendState.AlphaToCoverageEnable = false;
	Desc.BlendState.IndependentBlendEnable = false;

	if (Info.Blend.bIsEnabled)
	{
		Desc.BlendState.RenderTarget[0].BlendEnable = true;

		//color spec
		Desc.BlendState.RenderTarget[0].SrcBlend = static_cast<D3D12_BLEND>(Info.Blend.ColorPart.Src);
		Desc.BlendState.RenderTarget[0].DestBlend = static_cast<D3D12_BLEND>(Info.Blend.ColorPart.Dst);
		//?
		Desc.BlendState.RenderTarget[0].BlendOp = static_cast<D3D12_BLEND_OP>(Info.Blend.ColorPart.Op);

		//alpha spec
		Desc.BlendState.RenderTarget[0].SrcBlendAlpha = static_cast<D3D12_BLEND>(Info.Blend.AlphaPart.Src);
		Desc.BlendState.RenderTarget[0].DestBlendAlpha = static_cast<D3D12_BLEND>(Info.Blend.AlphaPart.Dst);
		//?
		Desc.BlendState.RenderTarget[0].BlendOpAlpha = static_cast<D3D12_BLEND_OP>(Info.Blend.AlphaPart.Op);
		
		Desc.BlendState.RenderTarget[0].RenderTargetWriteMask = Info.Blend.WriteMask;
				

	}

	if (Info.DepthStencil.bIsDepthEnabled)
	{
		Desc.DepthStencilState.DepthEnable = true;
		Desc.DepthStencilState.DepthWriteMask = static_cast<D3D12_DEPTH_WRITE_MASK>(Info.DepthStencil.DepthWriteMask);
		Desc.DepthStencilState.DepthFunc = static_cast<D3D12_COMPARISON_FUNC>(Info.DepthStencil.DepthFunc);

	}

	if (Info.DepthStencil.bIsStencilEnabled)
	{
		Desc.DepthStencilState.StencilEnable = true;
		Desc.DepthStencilState.StencilReadMask = Info.DepthStencil.StencilReadMask;
		Desc.DepthStencilState.StencilWriteMask = Info.DepthStencil.StencilWriteMask;

		if (auto *pFrontDesc = Info.DepthStencil.pStencilFrontface)
		{
			Desc.DepthStencilState.FrontFace.StencilDepthFailOp = static_cast<D3D12_STENCIL_OP>(pFrontDesc->DepthFailOp);
			Desc.DepthStencilState.FrontFace.StencilFailOp = static_cast<D3D12_STENCIL_OP>(pFrontDesc->StencilFailOp);
			Desc.DepthStencilState.FrontFace.StencilPassOp = static_cast<D3D12_STENCIL_OP>(pFrontDesc->StencilPassOp);
			Desc.DepthStencilState.FrontFace.StencilFunc = static_cast<D3D12_COMPARISON_FUNC>(pFrontDesc->StencilFunc);

		}

		if (auto *pBackDesc = Info.DepthStencil.pStencilBackface)
		{
			Desc.DepthStencilState.BackFace.StencilDepthFailOp = static_cast<D3D12_STENCIL_OP>(pBackDesc->DepthFailOp);
			Desc.DepthStencilState.BackFace.StencilFailOp = static_cast<D3D12_STENCIL_OP>(pBackDesc->StencilFailOp);
			Desc.DepthStencilState.BackFace.StencilPassOp = static_cast<D3D12_STENCIL_OP>(pBackDesc->StencilPassOp);
			Desc.DepthStencilState.BackFace.StencilFunc = static_cast<D3D12_COMPARISON_FUNC>(pBackDesc->StencilFunc);

		}


	}
#pragma endregion

	Desc.NumRenderTargets = 1;
	Desc.DSVFormat = static_cast<DXGI_FORMAT>(Info.DSVFormat);
	Desc.RTVFormats[0] = static_cast<DXGI_FORMAT>(Info.RTVFormat);

	Desc.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(Info.TopologyType);
	
	//Count 1, quality 0 == default / eg. no msaa
	Desc.SampleDesc.Count = 1;

	Desc.RasterizerState.FillMode = Info.bWireframeFaces ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	Desc.RasterizerState.FrontCounterClockwise = Info.bCCWIsFront;
	Desc.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(Info.CullMode);
	Desc.RasterizerState.DepthClipEnable = true;

	//Desc.InputLayout
	//Desc.pRootSignature
	//Desc.StreamOutput. ->> unsupported

}
*/