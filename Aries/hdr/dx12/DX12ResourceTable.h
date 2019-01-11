#pragma once
#include <memory>

//
#include "dx12/DX12DescriptorHeap.h"
#include "dx12/DX12Factory.h"


class IGAPIResource
{
public:
	//virtual ... GetType() const noexcept = 0;
	

};

struct FResourceTableDesc
{
public:
	IGAPIResource &Resource;
	//volatility flags

};

class FDX12ResourceTable
{
public:
	FDX12ResourceTable(FDX12Factory &Factory)
	{
		UINT Num{ 0 };
		m_pHeap = std::make_unique<FDX12DescriptorHeap>( Factory.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, Num, true );
		
		D3D12_DESCRIPTOR_RANGE1 r{};
		
		

	}

private:
	D3D12_ROOT_DESCRIPTOR_TABLE1 m_Desc;
	std::unique_ptr<FDX12DescriptorHeap> m_pHeap;

};
