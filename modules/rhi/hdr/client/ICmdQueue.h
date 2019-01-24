#pragma once
#include "EnumFlags.h"
#include "client/IFence.h"


class IResourceReserved;


enum class ECmdQueueType : char
{
	Direct,
	Compute,
	Copy

};

enum class EFTileRanges : int
{
	None = 0,
	Null = 1 << 0,
	Skip = 1 << 1,
	Reuse_Single = 1 << 2

};
GENERATE_BINARY_OPERATORS_FOR_ENUM_FLAGS(EFTileRanges);

struct FTileCoord
{
	unsigned int X, Y, Z, Subresource;

};

struct FTileRegion
{
	bool bUseBox;
	unsigned int NumTiles, Width;
	unsigned short Height, Depth;

};

struct FResourceRegion
{
	FTileCoord Coord;
	FTileRegion Region;

};

struct FTileRange
{
	unsigned int HeapOffset;
	unsigned int TileCount;
	EFTileRanges Flags;

};

class ICmdQueue
{
public:
	virtual ~ICmdQueue() = default;

	virtual void WaitFence(IFence &Fence) = 0;
	virtual void SignalFence(IFence &Fence) = 0;
	virtual void ResetFence(IFence &Fence) = 0;

	virtual void TileMappingsCopy(IResourceReserved &Dst, const FTileCoord &DstStartCoord, IResourceReserved &Src, const FTileCoord &SrcStartCoord, const FTileRegion &CopyRegion) = 0;
	virtual void TileMappingsUpdate(IResourceReserved &Resource, IHeap &Heap, unsigned int NumRegions, const FResourceRegion &Regions, unsigned int NumRanges, const FTileRange &TileRanges) = 0;

	//virtual void ExecuteCommandLists(unsigned int NumCmdLists, ICmdList &Lists) = 0;

	
};
