#pragma once
#include "dx12/DX12Factory.h"
#include "IHeap.h"


#include "Error.h"

class FDX12Heap : public IHeap
{
public:
	FDX12Heap(FDX12Factory &Factory, D3D12_HEAP_TYPE Type, size_t SizeInBytes, bool bHasMSAAAlignment) :
		m_SizeInBytes{ SizeInBytes }
	{
		D3D12_HEAP_DESC Desc{};
		Desc.Alignment = bHasMSAAAlignment ? D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
		Desc.SizeInBytes = m_SizeInBytes;
		//Assuming heap tier 2, check
		Desc.Flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
		
		D3D12_FEATURE_DATA_D3D12_OPTIONS options{};
		
		Factory.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));

		D3D12_HEAP_PROPERTIES Prop{};
		Prop.Type = Type;
		Prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		Prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		//CPU page property ?
		Desc.Properties = Prop;
		
		auto Hr = Factory.GetDevice()->CreateHeap(&Desc, IID_PPV_ARGS(&m_pHeap));
		ARI_THROW_IF_FAILED(Hr, "Could not create dx12 heap.");

		

	}

protected:
	ComPtr<ID3D12Heap> m_pHeap;
	size_t m_SizeInBytes;


};

