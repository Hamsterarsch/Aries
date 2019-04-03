#pragma once
#include <windows.h>//rework the os dependecy
#include <memory>
#include <vector>
#include "GAPITypes.h"

//Added for client convenience
#include "IWindow.h"
#include "IHeap.h"
#include "IReservedBuffer.h"
#include "client/ICmdQueue.h"

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
	
	virtual std::unique_ptr<IHeap> MakeHeap(EHeapTypes Type, size_t SizeInBytes, EResourceCategory TargetCategory, bool bHasMSAAAlignment) = 0;

	virtual std::unique_ptr<IReservedBuffer> MakeReservedBuffer(EBufferTypes Type, size_t SizeInBytes) = 0;

	//todo: implement interfaces for returned

	virtual std::unique_ptr<class FDX12CmdQueue> MakeCmdQueue(ECmdQueueType Type, int Priority, bool bHasGPUTimeoutEnabled) = 0;

	virtual std::unique_ptr<class FDX12CmdAllocator> MakeCmdAllocator(bool bForBundleRecording) = 0;

	virtual std::unique_ptr<class FDX12CmdList> MakeCmdList(const FDX12CmdAllocator &Allocator) = 0;

	virtual std::unique_ptr<class FDX12PlacedResource> MakePlacedBuffer(IHeap &Heap, size_t SizeInBytes, void *pData) = 0;

	virtual std::unique_ptr<class FDX12DescriptorTable> MakeDescriptorTable(const std::vector<struct FDescriptorInfo> &vDescriptorInfos, EDescriptorHeapTypes TableType) = 0;

	virtual std::unique_ptr<class FDX12RootSignature> MakeRootSignature(const struct FRootSignatureInfo &SignatureInfo) = 0;


};
