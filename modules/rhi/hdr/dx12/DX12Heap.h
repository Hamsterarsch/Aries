#pragma once
//#include <wrl/client.h>

#include "dx12/DX12Factory.h"
#include "client/IHeap.h"

#include "Error.h"

using Microsoft::WRL::ComPtr;

class FDX12Heap : public IHeap
{
public:
	FDX12Heap(FDX12Factory &Factory, EHeapTypes Type, size_t SizeInBytes, EResourceCategory TargetCategory, bool bHasMSAAAlignment) :
		m_SizeInBytes{ SizeInBytes },
		m_bHasMSAAAlignment{ bHasMSAAAlignment },
		m_HeapType{ Type },
		m_DefaultResourceState{ D3D12_RESOURCE_STATE_COMMON }
	{
		D3D12_HEAP_PROPERTIES Prop{};
		Prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		//CPU page property meaning?
		Prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

		switch (Type)
		{
		case EHeapTypes::Default:
			Prop.Type = D3D12_HEAP_TYPE_DEFAULT;
			break;
		case EHeapTypes::Readback:
			Prop.Type = D3D12_HEAP_TYPE_READBACK;
			m_DefaultResourceState = D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		case EHeapTypes::Upload:
			Prop.Type = D3D12_HEAP_TYPE_UPLOAD;
			m_DefaultResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
			break;
		default:
			ARI_THROW_ERROR(-1, "FDX12Heap, invalid heap type");

		}		
				
		D3D12_HEAP_DESC Desc{};
		Desc.Properties = Prop;
		Desc.SizeInBytes = m_SizeInBytes;
		Desc.Alignment = bHasMSAAAlignment ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		
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
		case EResourceCategory::All:
			Desc.Flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
			break;
		default:
			ARI_THROW_ERROR(-1, "FDX12Heap, invalid target category");

		}




		D3D12_FEATURE_DATA_D3D12_OPTIONS options{};		
		Factory.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));
		

		
		
		auto Hr = Factory.GetDevice()->CreateHeap(&Desc, IID_PPV_ARGS(&m_pHeap));
		ARI_THROW_IF_FAILED(Hr, "Could not create dx12 heap.");
		
		if (Type == EHeapTypes::Upload)
		{
			D3D12_RESOURCE_DESC RDesc{};
			RDesc.Width = RDesc.Height = RDesc.DepthOrArraySize = 1;
			RDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			RDesc.MipLevels = 1;
			RDesc.SampleDesc.Count = 1;
			RDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			ComPtr<ID3D12Resource> pHandleResource;
			Hr = Factory.GetDevice()->CreatePlacedResource(m_pHeap.Get(), 0, &RDesc, m_DefaultResourceState, nullptr, IID_PPV_ARGS(&pHandleResource));

			void * pData;
			D3D12_RANGE Range{};
			Hr = pHandleResource->Map(0, &Range, &pData);

			m_pHeapCurrent = m_pHeapStart = reinterpret_cast<UINT8 *>(pData);
			m_pHeapEnd = m_pHeapStart + m_SizeInBytes;

		}

	}

	size_t GetSizeInBytes() const noexcept final override { return m_SizeInBytes; }
	bool HasMSAAAlignment() const noexcept final override { return m_bHasMSAAAlignment; }
	EAPITypes GetAPIType() const noexcept final override { return EAPITypes::DX12; }
	ComPtr<ID3D12Heap> GetAPIHandle() const noexcept { return m_pHeap; }


	

	bool AllocatePlacedResource(FDX12Factory &Factory, ComPtr<ID3D12Resource> &out_pResource, D3D12_RESOURCE_DESC &Description, size_t SizeInBytes, void *pData)
	{		
		auto Alignment = m_bHasMSAAAlignment ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		auto SizeAligned = Align(SizeInBytes, Alignment);
		if( (m_SizeInBytes - m_CurrentOffset) < SizeInBytes )
		{
			return false;
		}

		auto Hr{ Factory.GetDevice()->CreatePlacedResource(m_pHeap.Get(), m_CurrentOffset, &Description, m_DefaultResourceState, nullptr, IID_PPV_ARGS(&out_pResource)) };
		if( FAILED(Hr) )
		{
			return false;
		}


		//Heap 'ptrs' are only initialized if heap type is upload, so the seperate offset track is used
		if( m_HeapType == EHeapTypes::Upload && pData )
		{
			memcpy(m_pHeapCurrent, pData, SizeInBytes);
			m_pHeapCurrent += SizeAligned;

		}

		m_CurrentOffset = min(m_CurrentOffset + SizeAligned, m_SizeInBytes);
		return true;

	}

	std::intptr_t Allocate(size_t SizeInBytes, void *pData) override
	{
		//return if not heap type upload

		auto Alignment = m_bHasMSAAAlignment ? 4'000'000 : 64;
		auto SizeAligned = Align(SizeInBytes, Alignment);

		if ( (m_pHeapEnd - m_pHeapCurrent) < SizeAligned )
		{
			nullptr;

		}

		memcpy(m_pHeapCurrent, pData, SizeInBytes);
		auto AllocationStart = m_pHeapCurrent;

		m_pHeapCurrent += SizeAligned;

		return reinterpret_cast<intptr_t>(AllocationStart);


	}


protected:
	size_t Align(size_t Size, size_t Alignment)
	{
		if( Alignment == 0 || ((Alignment - 1) & Alignment) )
		{
			ARI_THROW_ERROR(-1, "Non pow of two alignment");
		}
		return (Size + (Alignment - 1)) & ~(Alignment - 1);

	}

	ComPtr<ID3D12Heap> m_pHeap;
	size_t m_SizeInBytes;
	bool m_bHasMSAAAlignment;
	UINT8 *m_pHeapStart, *m_pHeapCurrent, *m_pHeapEnd;
	D3D12_RESOURCE_STATES m_DefaultResourceState;
	size_t m_CurrentOffset;
	EHeapTypes m_HeapType;
	

};

#include <vector>

class FDX12RingHeap : public FDX12Heap
{
public:
	FDX12RingHeap(FDX12Factory &Factory, EHeapTypes Type, size_t SizeInBytes, EResourceCategory TargetCategory, unsigned char InflightCount, bool bHasMSAAAlignment) :
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

