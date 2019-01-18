#include "Error.h"
#include "dx12/DX12Factory.h"
#include "dx12/DX12Fence.h"
#include "dx12/DX12CmdQueue.h"

FDX12CmdQueue::FDX12CmdQueue(class FDX12Factory &Factory, ECmdQueueType Type, int Priority, bool bHasGPUTimeoutEnabled)
{
	D3D12_COMMAND_QUEUE_DESC Desc{};
	Desc.Flags = bHasGPUTimeoutEnabled ? D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT : D3D12_COMMAND_QUEUE_FLAG_NONE;
	Desc.Priority = Priority;
	//First Adapter only
	Desc.NodeMask = 0;
	
	switch (Type)
	{
	case ECmdQueueType::Direct:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	case ECmdQueueType::Compute:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	case ECmdQueueType::Copy:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	default:
		ARI_THROW_ERROR(-1, "Dx12 cmd queue, invalid queue type");

	}

	ARI_THROW_IF_FAILED(
		Factory.GetDevice()->CreateCommandQueue(&Desc, IID_PPV_ARGS(&m_pCmdQueue)),
	"Could not create dx12 command queue"
	);


}

void FDX12CmdQueue::WaitFence(IFence &Fence)
{
	ARI_ASSERT(Fence.GetAPIType() == EAPITypes::DX12, "dx12 cmd queue api missmatch");
	auto Dx12Fence = static_cast<FDX12Fence &>(Fence);
	auto pAPIFence = Dx12Fence.GetAPIHandle();

	ARI_THROW_IF_FAILED(
		m_pCmdQueue->Wait(pAPIFence.Get(), 1),
	"Unsuccessful wait dx12 fence"
	);


}

void FDX12CmdQueue::SignalFence(IFence &Fence)
{
	ARI_ASSERT(Fence.GetAPIType() == EAPITypes::DX12, "dx12 cmd queue api missmatch");
	auto Dx12Fence = static_cast<FDX12Fence &>(Fence);
	auto pAPIFence = Dx12Fence.GetAPIHandle();

	ARI_THROW_IF_FAILED(
		m_pCmdQueue->Signal(pAPIFence.Get(), 1),
		"Unsuccessful wait dx12 fence"
	);


}

void FDX12CmdQueue::ResetFence(IFence &Fence)
{
	ARI_ASSERT(Fence.GetAPIType() == EAPITypes::DX12, "dx12 cmd queue api missmatch");
	auto Dx12Fence = static_cast<FDX12Fence &>(Fence);
	auto pAPIFence = Dx12Fence.GetAPIHandle();

	ARI_THROW_IF_FAILED(
		m_pCmdQueue->Signal(pAPIFence.Get(), 0),
		"Unsuccessful wait dx12 fence"
	);

}

void FDX12CmdQueue::TileMappingsCopy(IResourceReserved &Dst, const FTileCoord &DstStartCoord, IResourceReserved &Src, const FTileCoord &SrcStartCoord, const FTileRegion &CopyRegion)
{


}

void FDX12CmdQueue::TileMappingsUpdate(IResourceReserved &Resource, IHeap &Heap, unsigned int NumRegions, const FResourceRegion &Regions, unsigned int NumRanges, const FTileRange &TileRanges)
{


}

void FDX12CmdQueue::ExecuteCommandLists(unsigned int NumCmdLists, ICmdList &Lists)
{


}