#pragma once
#include "client/IGAPIResource.h"
#include "dx12/DX12Factory.h"
#include "dx12/DX12Heap.h"


class FDX12PlacedResource : public IGAPIResource
{
public:
	FDX12PlacedResource(FDX12Factory &Factory, IHeap &Heap, size_t SizeInBytes, void *pData, D3D12_RESOURCE_DESC &Desc) :
		m_SizeInBytes{ SizeInBytes }
	{
		ARI_ASSERT(Heap.GetAPIType() == EAPITypes::DX12, "Dx12 placed buffer api type mismatch");
		auto &APIHeap = static_cast<FDX12Heap &>(Heap);

		ARI_ASSERT(
			APIHeap.AllocatePlacedResource(Factory, m_pResource, Desc, m_SizeInBytes, pData),
		"Could not allocate dx12 placed resource from dx12 heap"
		);

		
	}

	ComPtr<ID3D12Resource> GetAPIHandle() const noexcept { return m_pResource; }

	UINT64 GetGPUAddress() const final override { return m_pResource->GetGPUVirtualAddress(); }

	size_t GetSizeInBytes() const noexcept final override { return m_SizeInBytes; }

protected:
	ComPtr<ID3D12Resource> m_pResource;
	size_t m_SizeInBytes;


};

