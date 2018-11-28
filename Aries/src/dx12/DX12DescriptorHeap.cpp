#include "Error.h"
#include "dx12\DX12DescriptorHeap.h"


FDX12DescriptorHeap::FDX12DescriptorHeap(ID3D12Device *pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, bool bIsShaderVisible) :
	m_Desc{}
{
	ARI_ASSERT(pDevice, "Trying to create dx12 descriptor heap with null device.");

	m_Desc.NumDescriptors = NumDescriptors;
	m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	m_Desc.Flags = (bIsShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	ARI_THROW_IF_FAILED(
		pDevice->CreateDescriptorHeap(&m_Desc, IID_PPV_ARGS(&m_pDh)),
		"Could not create dx12 rtv descriptor heap.");

	m_hCPUHeapStart = D3D12_CPU_DESCRIPTOR_HANDLE{ m_pDh->GetCPUDescriptorHandleForHeapStart() };
	m_hGPUHeapStart = D3D12_GPU_DESCRIPTOR_HANDLE{ m_pDh->GetGPUDescriptorHandleForHeapStart() };
	m_HandleIncrementSize = pDevice->GetDescriptorHandleIncrementSize(m_Desc.Type);


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