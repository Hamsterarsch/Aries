#pragma once
#include <windows.h>//rework the os dependecy
#include <memory>
#include "GAPITypes.h"

//Added for client convenience
#include "IWindow.h"
#include "IHeap.h"
#include "IReservedBuffer.h"

//Typeless enum memory
/*
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
*/



enum class EOSType : unsigned short
{
	Windows,
	Apple,
	Linux

};

class IGAPIFactory
{
public:
	virtual ~IGAPIFactory() = default;

	virtual EAPITypes GetApiType() const = 0;

	virtual std::unique_ptr<IWindow> MakeWindow(UINT Width, UINT Height, LPCWSTR pWindowName, WNDPROC pfnWndProc, LPCWSTR pClassName) = 0;
	
	virtual std::unique_ptr<IHeap> MakeHeap(EHeapType Type, size_t SizeInBytes, EResourceCategory TargetCategory, bool bHasMSAAAlignment) = 0;

	virtual std::unique_ptr<IReservedBuffer> MakeReservedBuffer(EBufferTypes Type, size_t SizeInBytes) = 0;

};
