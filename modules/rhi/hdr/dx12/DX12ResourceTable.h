#pragma once
#include <memory>
#include <vector>

//
#include "dx12/DX12DescriptorHeap.h"
#include "dx12/DX12Factory.h"
#include "client/IGAPIResource.h"
#include "Error.h"
#include "client/GAPITypes.h"


enum class EDescriptorTypes : char 
{
	None,
	CBV,
	SRV,
	UAV,
	RTV,
	DSV,
	Sampler

};

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

struct FSRVArray
{
	unsigned int NumElements;
	//For cube arrays the first array slice is the index of the first 2D texture to use
	unsigned int FirstArraySliceToUse;

};

struct FDescriptorInfo
{
public:
	IGAPIResource &Resource;
	EDescriptorTypes Type;
	unsigned int OverrideIndex;
	FDX12DescriptorTable *pOverrideSource;
	ESRVDimensions SRVDimension;
	EGAPIFormats Format;
	union
	{
		FSRVTexDesc Tex;
		FSRVBufferDesc Buffer;
	};
	FSRVArray Array;

	//volatility flags
	
};



class FDX12DescriptorTable
{
public:
	FDX12DescriptorTable(FDX12Factory &Factory, const std::vector<FDescriptorInfo> &vDescriptorInfos, EDescriptorHeapTypes TableType)
	{
		UINT Num{ 0 };
		
		switch (TableType)
		{
		case EDescriptorHeapTypes::CBV_SRV_UAV:
			m_APIHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			break;
		case EDescriptorHeapTypes::RTV:
			m_APIHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			break;
		case EDescriptorHeapTypes::DSV:
			m_APIHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			break;
		case EDescriptorHeapTypes::Sampler:
			m_APIHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			break;

		}
		m_pHeap = std::make_unique<FDX12DescriptorHeap>( Factory, m_APIHeapType, static_cast<UINT>( vDescriptorInfos.size() ), true );
		
		std::vector<D3D12_DESCRIPTOR_RANGE1> vDescriptorRanges(vDescriptorInfos.size());
		{
			unsigned int DescriptorIndex{ 0 };
			for (auto &&DescriptorInfo : vDescriptorInfos)
			{				
				D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc{};
				//CBVDesc.

				//Factory.GetDevice()->CreateConstantBufferView()
				//switch( DescriptorInfo.Resource.GetType() )
				{
				
				
				}

				++DescriptorIndex;
			}

		}
		
		

	}

protected:
	std::intptr_t GetDescriptorAt(unsigned int Index)
	{
		return m_pHeap->GetHandleCPU(Index).ptr;
		
	}

	void CheckAndDoDescriptorOverride(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		ARI_ASSERT(Info.pOverrideSource->m_APIHeapType == m_APIHeapType, "Dx12 descriptor table override is of different heap type than target table");

		D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle{};
		SrcHandle.ptr = Info.pOverrideSource->GetDescriptorAt(Info.OverrideIndex);
		Factory.GetDevice()->CopyDescriptorsSimple(1, m_pHeap->GetHandleCPU(DescriptorIndex), SrcHandle, m_APIHeapType);
				
		
	}

	void CreateCBV(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc{};
		Desc.BufferLocation = Info.Resource.GetGPUAddress();
		Desc.SizeInBytes = Info.Resource.GetSizeInBytes();
		
		Factory.GetDevice()->CreateConstantBufferView(&Desc, m_pHeap->GetHandleCPU(DescriptorIndex));
		

	}

	void CreateSRV(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		//D3D12_SHADER_COMPONENT_MAPPING pending

		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		ARI_ASSERT(Info.SRVDimension != ESRVDimensions::None, "Dx12 resource table constructed with srv dimension of type none");
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc{};		

		switch (Info.SRVDimension)
		{
		case ESRVDimensions::Buffer:
			//Desc.Format = implement this as needed
			Desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			Desc.Buffer.FirstElement = Info.Buffer.FirstElementIndex;
			Desc.Buffer.NumElements = Info.Buffer.NumElements;
			Desc.Buffer.StructureByteStride = Info.Buffer.StructByteStride;
			Desc.Buffer.Flags = Info.Buffer.bIsRaw ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;
						
			break;
		case ESRVDimensions::Tex1D:
			Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			break;
		case ESRVDimensions::Tex2D:
			Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			break;
		case ESRVDimensions::Tex3D:
			Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			break;
		case ESRVDimensions::TexCube:
			Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			break;
		case ESRVDimensions::Tex1DArray:
			Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			break;
		case ESRVDimensions::Tex2DArray:
			Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			break;
		case ESRVDimensions::TexCubeArray:
			Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
			break;
		default:
			ARI_THROW_ERROR(-1, "Dx12 resource table, unknown srv dimension");
			break;
		}
		
		//Texture srvs
		if (Info.SRVDimension != ESRVDimensions::Buffer)
		{
			//Sub structures are placend in a union so this is valid for all other texture structures,
			//at least for MostDetailedMip and MipLevels (because they always are the first two members)
			Desc.Texture1D.MostDetailedMip = Info.Tex.LowestMipToUse;
			if (Info.Tex.HighestMipToUse < 0)
			{
				Desc.Texture1D.MipLevels = -1;
			}
			else
			{
				Desc.Texture1D.MipLevels = Info.Tex.HighestMipToUse - Info.Tex.LowestMipToUse;
			}
		}

		//Texture array srvs
		if 
		(
			Info.SRVDimension == ESRVDimensions::Tex1DArray 
			|| Info.SRVDimension == ESRVDimensions::Tex2DArray 
			|| Info.SRVDimension == ESRVDimensions::TexCubeArray
		)
		{
			Desc.Texture1DArray.ArraySize = Info.Array.NumElements;
			//For cube arrays the first array slice is the index of the first 2D texture to use
			Desc.Texture1DArray.FirstArraySlice = Info.Array.FirstArraySliceToUse;
		}

		Factory.GetDevice()->CreateShaderResourceView(..., &Desc, m_pHeap->GetHandleCPU(DescriptorIndex));

	}

	void CreateUAV(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc{};
		

	}

	void CreateDSV(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_DEPTH_STENCIL_DESC1 Desc{};


	}

	void CreateRTV(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_RENDER_TARGET_VIEW_DESC Desc{};


	}

	void CreateSampler(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_SAMPLER_DESC Desc{};

	}


	 

	D3D12_ROOT_DESCRIPTOR_TABLE1 m_Desc;
	D3D12_DESCRIPTOR_HEAP_TYPE m_APIHeapType;
	std::unique_ptr<FDX12DescriptorHeap> m_pHeap;

};
