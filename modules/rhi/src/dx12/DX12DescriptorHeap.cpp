#include "Error.h"
#include "dx12\DX12DescriptorHeap.h"


FDX12DescriptorHeap::FDX12DescriptorHeap(FDX12Factory &Factory, EDescriptorHeapTypes Type, UINT DescriptorCapacity, bool bIsShaderVisible) :
	m_Desc{}
{
	m_Desc.NumDescriptors = DescriptorCapacity;
	m_Desc.Flags = (bIsShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	
	switch (Type)
	{
	case EDescriptorHeapTypes::CBV_SRV_UAV:
		m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	case EDescriptorHeapTypes::DSV:
		m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	case EDescriptorHeapTypes::RTV:
		m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	case EDescriptorHeapTypes::Sampler:
		m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	default:
		ARI_THROW_ERROR(-1, "Dx12 descriptor heap, invalid descriptor heap type");
	}

	ARI_THROW_IF_FAILED(
		Factory.GetDevice()->CreateDescriptorHeap(&m_Desc, IID_PPV_ARGS(&m_pHeap)),
	"Could not create dx12 descriptor heap.");

	m_hCPUHeapStart = D3D12_CPU_DESCRIPTOR_HANDLE{ m_pHeap->GetCPUDescriptorHandleForHeapStart() };
	m_hGPUHeapStart = D3D12_GPU_DESCRIPTOR_HANDLE{ m_pHeap->GetGPUDescriptorHandleForHeapStart() };
	m_HandleIncrementSize = Factory.GetDevice()->GetDescriptorHandleIncrementSize(m_Desc.Type);

	
	
	
}

D3D12_CPU_DESCRIPTOR_HANDLE FDX12DescriptorHeap::GetHandleCPU(UINT Index) const
{
	ARI_ASSERT(Index < m_Desc.NumDescriptors, "Dx12 descriptor heap cpu handle out of range.");

	D3D12_CPU_DESCRIPTOR_HANDLE Handle{m_hCPUHeapStart};
	Handle.ptr += Index * m_HandleIncrementSize;
	return Handle;


}

D3D12_GPU_DESCRIPTOR_HANDLE FDX12DescriptorHeap::GetHandleGPU(UINT Index) const
{
	ARI_ASSERT(Index < m_Desc.NumDescriptors, "Dx12 descriptor heap gpu handle out of range.");

	D3D12_GPU_DESCRIPTOR_HANDLE Handle{ m_hGPUHeapStart };
	Handle.ptr += Index * m_HandleIncrementSize;
	return Handle;


}