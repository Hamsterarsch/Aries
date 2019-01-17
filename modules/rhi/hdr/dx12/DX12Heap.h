#pragma once
#include "dx12/DX12Factory.h"
#include "client/IHeap.h"


#include "Error.h"

class FDX12Heap : public IHeap
{
public:
	FDX12Heap(FDX12Factory &Factory, EHeapType Type, size_t SizeInBytes, EResourceCategory TargetCategory, bool bHasMSAAAlignment) :
		m_SizeInBytes{ SizeInBytes }
	{
		D3D12_HEAP_PROPERTIES Prop{};
		Prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		//CPU page property meaning?
		Prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

		switch (Type)
		{
		case EHeapType::Default:
			Prop.Type = D3D12_HEAP_TYPE_DEFAULT;
			break;
		case EHeapType::Readback:
			Prop.Type = D3D12_HEAP_TYPE_READBACK;
			break;
		case EHeapType::Upload:
			Prop.Type = D3D12_HEAP_TYPE_UPLOAD;
			break;
		default:
			ARI_THROW_ERROR(-1, "FDX12Heap, invalid heap type");

		}		
				
		D3D12_HEAP_DESC Desc{};
		Desc.Properties = Prop;
		Desc.SizeInBytes = m_SizeInBytes;
		Desc.Alignment = bHasMSAAAlignment ? D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;

		switch (TargetCategory)
		{
		case EResourceCategory::Buffer:
			Desc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
			break;
		case EResourceCategory::Texture:
			Desc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;
			break;
		case EResourceCategory::RT_DS_Texture:
			Desc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
			break;
		default:
			ARI_THROW_ERROR(-1, "FDX12Heap, invalid target category");

		}




		D3D12_FEATURE_DATA_D3D12_OPTIONS options{};		
		Factory.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));
		

		
		
		auto Hr = Factory.GetDevice()->CreateHeap(&Desc, IID_PPV_ARGS(&m_pHeap));
		ARI_THROW_IF_FAILED(Hr, "Could not create dx12 heap.");

		

	}

	size_t GetSizeInBytes() const noexcept final override { return m_SizeInBytes; }

protected:
	ComPtr<ID3D12Heap> m_pHeap;
	size_t m_SizeInBytes;


};

#include <vector>

class FDX12RingHeap : public FDX12Heap
{
public:
	FDX12RingHeap(FDX12Factory &Factory, EHeapType Type, size_t SizeInBytes, EResourceCategory TargetCategory, unsigned char InflightCount, bool bHasMSAAAlignment) :
		FDX12Heap{ Factory, Type, SizeInBytes, TargetCategory, bHasMSAAAlignment },
		m_vFrameOffsetsStart(InflightCount),
		m_vFrameOffsetsCurrent(InflightCount)
	{
		

	}

	size_t AllocForInflight(unsigned char InflightIndex, size_t SizeInBytes)
	{
		if (SizeInBytes > m_SizeInBytes)
		{
			ARI_THROW_ERROR(-1, "FDX12RingHeap out of memory");

		}
		
		auto OffsetStart{ m_vFrameOffsetsStart.at(InflightIndex) };
		auto OffsetEnd{ m_vFrameOffsetsCurrent.at(InflightIndex) };



	}

protected:
	std::vector<size_t> m_vFrameOffsetsStart,
						m_vFrameOffsetsCurrent;
	


};

