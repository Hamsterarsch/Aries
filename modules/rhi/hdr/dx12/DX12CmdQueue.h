#pragma once
#include <wrl/client.h>
#include "EnumFlags.h"
#include "client/ICmdQueue.h"


using Microsoft::WRL::ComPtr;

class FDX12CmdQueue : public ICmdQueue
{
public:
	FDX12CmdQueue(class FDX12Factory &Factory, ECmdQueueType Type, int Priority, bool bHasGPUTimeoutEnabled);

	virtual void WaitFence(IFence &Fence) override;
	virtual void SignalFence(IFence &Fence) override;
	virtual void ResetFence(IFence &Fence) override;

	virtual void TileMappingsCopy(IResourceReserved &Dst, const FTileCoord &DstStartCoord, IResourceReserved &Src, const FTileCoord &SrcStartCoord, const FTileRegion &CopyRegion) override;
	virtual void TileMappingsUpdate(IResourceReserved &Resource, IHeap &Heap, unsigned int NumRegions, const FResourceRegion &Regions, unsigned int NumRanges, const FTileRange &TileRanges) override;

	virtual void ExecuteCommandLists(unsigned int NumCmdLists, ICmdList &Lists) override;


protected:
	ComPtr<class ID3D12CommandQueue> m_pCmdQueue;


};

