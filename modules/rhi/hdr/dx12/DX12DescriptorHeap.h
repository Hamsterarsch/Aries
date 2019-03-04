#pragma once
#include <wrl/client.h>
#include "dx12/DX12Factory.h"
#include "dx12/DX12TypeTranslations.h"

using Microsoft::WRL::ComPtr;

class FDX12DescriptorHeap
{
public:
	FDX12DescriptorHeap(FDX12Factory &Factory, EDescriptorHeapTypes Type, UINT DescriptorCapacity, bool bIsShaderVisible = false);
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU(UINT Index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(UINT Index) const;


protected:
	D3D12_DESCRIPTOR_HEAP_DESC m_Desc;
	ComPtr<ID3D12DescriptorHeap> m_pHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hCPUHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE m_hGPUHeapStart;
	UINT m_HandleIncrementSize;

	
};