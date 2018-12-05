#pragma once
#include "IWindow.h"

//Typeless enum memory
using TEM = unsigned short;

struct FBlend
{
	TEM Dst,
		Src,
		Op;

};

struct FBlendState
{
	bool bIsEnabled;	
	FBlend ColorPart,
		   AlphaPart;
	UINT8 WriteMask;


};

struct FStencilOperations
{
	TEM StencilFailOp,
		DepthFailOp,
		StencilPassOp,
		StencilFunc;

};

struct FDepthStencilState
{
	bool bIsDepthEnabled,
		 bIsStencilEnabled;
	TEM DepthFunc;
	UINT8 DepthWriteMask,
		  StencilReadMask,
		  StencilWriteMask;

	FStencilOperations *pStencilFrontface,
		*pStencilBackface;

};

struct FShaderState
{


};

struct FPSOInfo
{
	FBlendState Blend;
	FDepthStencilState DepthStencil;
	FShaderState Shader;
	unsigned int DSVFormat;
	unsigned int RTVFormat;
	TEM TopologyType,
		CullMode;
	bool bWireframeFaces,
		 bCCWIsFront;
	

};

enum class EAPIType : unsigned short
{
	DX12

};

enum class EOSType : unsigned short
{
	Windows

};

class IGAPIFactory
{
public:
	virtual EAPIType GetApiType() const = 0;

	virtual std::unique_ptr<IWindow> MakeWindow(UINT Width, UINT Height, LPCWSTR pWindowName, WNDPROC pfnWndProc, LPCWSTR pClassName) = 0;


};
