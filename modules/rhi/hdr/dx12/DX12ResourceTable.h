#pragma once
#include <memory>
#include <vector>

#include "dx12/DX12DescriptorHeap.h"
#include "dx12/DX12Factory.h"
#include "client/IDX12Resource.h"
#include "Error.h"
#include "dx12/DX12TypeTranslations.h"


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

struct FSamplerDesc
{
	ETexFilterMode FilterMode;
	ETexAddressMode U,
					V,
					W;
	unsigned int LowestMipToUse;
	unsigned int HighestMiptoUse;//uint max for all available (?)
	int MipBias;
	unsigned int MaxAnisotropyLevel;


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
		FSRVArrayDesc Array;
		FDSVDesc DSV;
		FSamplerDesc Sampler;
	};


};

struct FSignatureLayoutDesc
{

	EFShaderStages ShaderVisibility;
	struct FSignatureSrcDesc;
	{
		//source reference
		unsigned int StartIndex;
		unsigned int StartShaderRegister;
		unsigned int RangeSize;//uint max for unbounded
		EFDescriptorFlags RangeFlags;
		//range flags 

	};

};

class FDX12SignatureLayout
{
public:
	FDX12SignatureLayout()
	{

	}

};

class FDX12DescriptorTable
{
public:
	FDX12DescriptorTable(FDX12Factory &Factory, const std::vector<FDescriptorInfo> &vDescriptorInfos, EDescriptorHeapTypes TableType)
	{
		m_pHeap = std::make_unique<FDX12DescriptorHeap>( Factory, FDX12TranslateHeapType(TableType), static_cast<UINT>( vDescriptorInfos.size() ), true );
		
		std::vector<D3D12_DESCRIPTOR_RANGE1> vDescriptorRanges(vDescriptorInfos.size());
		
		
		
			

		unsigned int DescriptorIndex{ 0 };
		for (auto &&DescriptorInfo : vDescriptorInfos)
		{				
			ARI_ASSERT(DescriptorInfo.Resource.GetAPIType() == EAPITypes::DX12, "Dx12 descriptor table, resource api mismatch");

			switch (DescriptorInfo.Type)
			{
			case EDescriptorTypes::CBV:
				CreateCBV(Factory, DescriptorIndex, DescriptorInfo);
				break;
			case EDescriptorTypes::SRV:
				CreateSRV(Factory, DescriptorIndex, DescriptorInfo);
				break;
			case EDescriptorTypes::UAV:
				CreateUAV(Factory, DescriptorIndex, DescriptorInfo);
				break;
			case EDescriptorTypes::RTV:
				CreateRTV(Factory, DescriptorIndex, DescriptorInfo);
				break;
			case EDescriptorTypes::DSV:
				CreateDSV(Factory, DescriptorIndex, DescriptorInfo);
				break;
			case EDescriptorTypes::Sampler:
				CreateSampler(Factory, DescriptorIndex, DescriptorInfo);
				break;
			default:
				ARI_THROW_ERROR(-1, "Dx12 resource table invalid descriptor type in decriptor info");
				break;
			}
			++DescriptorIndex;


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
		Desc.Format = DX12TranslateFormat(Info.Format);
		Desc.ViewDimension = DX12TranslateSRVDimension(Info.SRVDimension);

		//buffer srvs
		if (Info.SRVDimension == ESRVDimensions::Buffer)
		{
			Desc.Buffer.FirstElement = Info.Buffer.FirstElementIndex;
			Desc.Buffer.NumElements = Info.Buffer.NumElements;
			Desc.Buffer.StructureByteStride = Info.Buffer.StructByteStride;
			Desc.Buffer.Flags = Info.Buffer.bIsRaw ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;
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

		auto &APIResource = static_cast<IDX12Resource &>(Info.Resource);
		Factory.GetDevice()->CreateShaderResourceView(APIResource.GetAPIHandle().Get(), &Desc, m_pHeap->GetHandleCPU(DescriptorIndex));


	}

	void CreateUAV(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc{};
		
		ARI_THROW_ERROR(-1, "Dx12 descriptor table uav not implemented");


	}

	//Only 2d depth views with mip slice 0
	void CreateDSV(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_DEPTH_STENCIL_VIEW_DESC Desc{};
		Desc.Format = DX12TranslateFormat(Info.Format);
		D3D12_DSV_FLAGS Flags{};
		Flags |= Info.DSV.bIsDepthReadOnly ? D3D12_DSV_FLAG_READ_ONLY_DEPTH : D3D12_DSV_FLAG_NONE;
		Flags |= Info.DSV.bIsStencilReadOnly ? D3D12_DSV_FLAG_READ_ONLY_STENCIL : D3D12_DSV_FLAG_NONE;
		Desc.Flags = Flags;
		Desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		//No mip spec for depth views
		Desc.Texture2D.MipSlice = 0;

		auto &APIResource = static_cast<IDX12Resource &>(Info.Resource);
		Factory.GetDevice()->CreateDepthStencilView(APIResource.GetAPIHandle().Get(), &Desc, m_pHeap->GetHandleCPU(DescriptorIndex));


	}

	//Only 2d rtvs with mip and plane slice both 0
	void CreateRTV(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_RENDER_TARGET_VIEW_DESC Desc{};
		Desc.Format = DX12TranslateFormat(Info.Format);
		Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//Mip and plane slice stay 0
		//Desc.Texture2D

		auto &APIResource = static_cast<IDX12Resource &>(Info.Resource);
		Factory.GetDevice()->CreateRenderTargetView(APIResource.GetAPIHandle().Get(), &Desc, m_pHeap->GetHandleCPU(DescriptorIndex));


	}

	void CreateSampler(FDX12Factory &Factory, unsigned int DescriptorIndex, FDescriptorInfo &Info)
	{
		CheckAndDoDescriptorOverride(Factory, DescriptorIndex, Info);

		D3D12_SAMPLER_DESC Desc{};
		Desc.AddressU = DX12TranslateAddressMode(Info.Sampler.U);
		Desc.AddressV = DX12TranslateAddressMode(Info.Sampler.V);
		Desc.AddressW = DX12TranslateAddressMode(Info.Sampler.W);
		Desc.Filter = DX12TranslateFilterMode(Info.Sampler.FilterMode);
		Desc.MaxAnisotropy = Info.Sampler.MaxAnisotropyLevel;
		//Integer spec only
		Desc.MinLOD = Info.Sampler.LowestMipToUse;
		Desc.MaxLOD = Info.Sampler.HighestMiptoUse;
		Desc.MipLODBias = Info.Sampler.MipBias;
		//Border always black
		//Comparision func pending
		//Desc.ComparisonFunc = DX12TranslateComparisonFunc(Info.Sampler.)

		Factory.GetDevice()->CreateSampler(&Desc, m_pHeap->GetHandleCPU(DescriptorIndex));


	}


	 

	D3D12_ROOT_DESCRIPTOR_TABLE1 m_Desc;
	D3D12_DESCRIPTOR_HEAP_TYPE m_APIHeapType;
	std::unique_ptr<FDX12DescriptorHeap> m_pHeap;

};
