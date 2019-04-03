#pragma once
#include <memory>
#include <vector>

#include "dx12/DX12DescriptorHeap.h"
#include "dx12/DX12Factory.h"
#include "Error.h"
#include "dx12/DX12TypeTranslations.h"
#include "IDX12Resource.h"


struct FSRVTexDesc
{
	unsigned int LowestMipToUse;

	int HighestMipToUse;


};

struct FSRVBufferDesc
{
	unsigned int	NumElements,
					FirstElementIndex;

	bool bIsRaw;

	unsigned int StructByteStride;


};

struct FSRVArrayDesc
{
	unsigned int NumElements;

	//For cube arrays the first array slice is the index of the first 2D texture to use
	unsigned int FirstArraySliceToUse;


};

struct FDSVDesc
{
	bool bIsDepthReadOnly,

		 bIsStencilReadOnly;


};



struct FDescriptorInfo
{
public:
	std::weak_ptr<IGAPIResource> pResource;

	EDescriptorTypes Type;

	unsigned int OverrideIndex;

	class FDX12DescriptorTable *pOverrideSource;

	ESRVDimensions SRVDimension;

	EGAPIFormats Format;

	union
	{
		FSRVTexDesc Tex;

		FSRVBufferDesc Buffer;

		FSRVArrayDesc Array;

		FDSVDesc DSV;

		FSamplerDesc Sampler;
	};


};


class FDX12DescriptorTable
{
public:
	FDX12DescriptorTable(FDX12Factory &Factory, const std::vector<FDescriptorInfo> &vDescriptorInfos, EDescriptorHeapTypes TableType);

	unsigned int GetNumEntries() const noexcept;

	const std::vector<D3D12_DESCRIPTOR_RANGE1> &GetPartialRangeInfos() const noexcept;

	bool IsValid() const noexcept;


protected:
	std::intptr_t GetDescriptorAt(unsigned int Index) const;

	bool CheckAndDoDescriptorOverride(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info);

	//preconditions for create functions:
	//the resource referenced inside the FDescriptorInfo must be a subtype of IDX12Resource
	//locking the weak pointer inside FDescriptorInfo referencing the resource must yield a valid reference	

	void CreateCBV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info);

	void CreateSRV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info);

	void CreateUAV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info);

	//Only 2d depth views with mip slice 0
	void CreateDSV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info);

	//Only 2d rtvs with mip and plane slice both 0
	void CreateRTV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info);

	void CreateSampler(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info);


	std::vector<D3D12_DESCRIPTOR_RANGE1> m_vPartialRangeDescs;

	D3D12_DESCRIPTOR_HEAP_TYPE m_APIHeapType;

	std::unique_ptr<FDX12DescriptorHeap> m_pHeap;

	bool m_bIsValid;


};
