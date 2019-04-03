#include "dx12/DX12DescriptorTable.h"

FDX12DescriptorTable::FDX12DescriptorTable(FDX12Factory &Factory, const std::vector<FDescriptorInfo> &vDescriptorInfos, EDescriptorHeapTypes TableType) :
	m_vPartialRangeDescs(vDescriptorInfos.size()),
	m_bIsValid{ true }
{
	m_pHeap = std::make_unique<FDX12DescriptorHeap>(Factory, TableType, static_cast<UINT>(vDescriptorInfos.size()), true);
	
	unsigned int DescriptorIndex{ 0 };
	for (auto &&DescriptorInfo : vDescriptorInfos)
	{
		auto pResourceLock{ DescriptorInfo.pResource.lock() };
		if(!pResourceLock)
		{
			m_bIsValid = false;
			m_pHeap.reset(nullptr);
			return;
		}
		ARI_ASSERT(pResourceLock->GetAPIType() == EAPITypes::DX12, "Dx12 descriptor table, resource api mismatch");
			   
		if (CheckAndDoDescriptorOverride(Factory, DescriptorIndex, DescriptorInfo))
		{
			continue;
		}
		
		m_vPartialRangeDescs.at(DescriptorIndex).NumDescriptors = 1;
		m_vPartialRangeDescs.at(DescriptorIndex).OffsetInDescriptorsFromTableStart = DescriptorIndex;
		m_vPartialRangeDescs.at(DescriptorIndex).RangeType = DX12TranslateDescriptorTypes(DescriptorInfo.Type);

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
		}
		++DescriptorIndex;

	}


}

unsigned int FDX12DescriptorTable::GetNumEntries() const noexcept
{
	return m_vPartialRangeDescs.size();


}

const std::vector<D3D12_DESCRIPTOR_RANGE1>& FDX12DescriptorTable::GetPartialRangeInfos() const noexcept
{
	return m_vPartialRangeDescs;


}

bool FDX12DescriptorTable::IsValid() const noexcept
{
	return m_bIsValid;


}


//Protected-------------------------------

std::intptr_t FDX12DescriptorTable::GetDescriptorAt(unsigned int Index) const
{
	return m_pHeap->GetHandleCPU(Index).ptr;


}

bool FDX12DescriptorTable::CheckAndDoDescriptorOverride(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info)
{
	if (Info.pOverrideSource)
	{
		//todo: use logging + validity 
		ARI_ASSERT(Info.pOverrideSource->IsValid(), "Dx12 descriptor table override source is invalid");
		ARI_ASSERT(Info.pOverrideSource->m_pHeap->GetHeapType() == this->m_pHeap->GetHeapType() ,"Dx12 descriptor table override is of different heap type than target table");

		D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle{};
		SrcHandle.ptr = Info.pOverrideSource->GetDescriptorAt(Info.OverrideIndex);
		Factory.GetDevice()->CopyDescriptorsSimple(1, m_pHeap->GetHandleCPU(DescriptorIndex), SrcHandle, m_APIHeapType);
		return true;
	}
	return false;


}

void FDX12DescriptorTable::CreateCBV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc{};

	auto pResource{ Info.pResource.lock() };
	Desc.BufferLocation = pResource->GetGPUAddress();
	Desc.SizeInBytes = pResource->GetSizeInBytes();

	Factory.GetDevice()->CreateConstantBufferView(&Desc, m_pHeap->GetHandleCPU(DescriptorIndex));


}

void FDX12DescriptorTable::CreateSRV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info)
{
	//D3D12_SHADER_COMPONENT_MAPPING pending

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

	
	auto pResource{ Info.pResource.lock() };
	auto *pAPIResource{ static_cast<IDX12Resource *>(pResource.get()) };

	Factory.GetDevice()->CreateShaderResourceView(pAPIResource->GetAPIHandle().Get(), &Desc, m_pHeap->GetHandleCPU(DescriptorIndex));


}

void FDX12DescriptorTable::CreateUAV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC Desc{};

	ARI_THROW_ERROR(-1, "Dx12 descriptor table uav not implemented");


}

//Only 2d depth views with mip slice 0
void FDX12DescriptorTable::CreateDSV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC Desc{};
	Desc.Format = DX12TranslateFormat(Info.Format);
	D3D12_DSV_FLAGS Flags{};
	Flags |= Info.DSV.bIsDepthReadOnly ? D3D12_DSV_FLAG_READ_ONLY_DEPTH : D3D12_DSV_FLAG_NONE;
	Flags |= Info.DSV.bIsStencilReadOnly ? D3D12_DSV_FLAG_READ_ONLY_STENCIL : D3D12_DSV_FLAG_NONE;
	Desc.Flags = Flags;
	Desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//No mip spec for depth views
	Desc.Texture2D.MipSlice = 0;

	auto pResource{ Info.pResource.lock() };
	auto *pAPIResource{ static_cast<IDX12Resource *>(pResource.get()) };

	Factory.GetDevice()->CreateDepthStencilView(pAPIResource->GetAPIHandle().Get(), &Desc, m_pHeap->GetHandleCPU(DescriptorIndex));


}

//Only 2d rtvs with mip and plane slice both 0
void FDX12DescriptorTable::CreateRTV(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info)
{
	D3D12_RENDER_TARGET_VIEW_DESC Desc{};
	Desc.Format = DX12TranslateFormat(Info.Format);
	Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//Mip and plane slice stay 0
	//Desc.Texture2D

	auto pResource{ Info.pResource.lock() };
	auto *pAPIResource{ static_cast<IDX12Resource *>(pResource.get()) };

	Factory.GetDevice()->CreateRenderTargetView(pAPIResource->GetAPIHandle().Get(), &Desc, m_pHeap->GetHandleCPU(DescriptorIndex));


}

void FDX12DescriptorTable::CreateSampler(FDX12Factory &Factory, unsigned int DescriptorIndex, const FDescriptorInfo &Info)
{
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