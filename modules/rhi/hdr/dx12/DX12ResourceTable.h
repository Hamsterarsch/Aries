#pragma once
#include <memory>
#include <vector>

//
#include "dx12/DX12DescriptorHeap.h"
#include "dx12/DX12Factory.h"
//#include "GAPITypes.h"


class IGAPIResource
{
public:
	//virtual EGAPIResourceTypes GetType() const noexcept = 0;
	

};

struct FDescriptorInfo
{
public:
	IGAPIResource &Resource;
	//volatility flags

};

class FDX12ResourceTable
{
public:
	FDX12ResourceTable(FDX12Factory &Factory, const std::vector<FDescriptorInfo> &vDescriptorInfos)
	{
		UINT Num{ 0 };
		m_pHeap = std::make_unique<FDX12DescriptorHeap>( Factory.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, static_cast<UINT>( vDescriptorInfos.size() ), true );
		
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

private:
	D3D12_ROOT_DESCRIPTOR_TABLE1 m_Desc;
	std::unique_ptr<FDX12DescriptorHeap> m_pHeap;

};
