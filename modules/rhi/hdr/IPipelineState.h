#pragma once
#include "LinkageMacro.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
using uint = unsigned int;

/*
struct FBlendState
{
	 uint BlendOp;
	 uint SrcAlpha;
	 uint DstAlpha;

};

struct FStencilOperations
{
	uint StencilFailOp;
	uint StencilPassOp;
	uint DepthFailOp;
	uint DepthPassOp;

};

struct FDepthStencilState
{
	uint DepthOp;
	uint DepthWriteMask;
	uint StencilReadMask;
	FStencilOperations StencilFrontface;
	FStencilOperations StencilBackface;

};
*/

class IPipelineState
{
public:
	//virtual void BindBlendState(uint Slot, FBlendState &&mov_BlendState) = 0;
	//virtual void BindDepthStencilState(uint Slot, FDepthStencilState &&mov_DepthStencilState) = 0;
	
	
};
