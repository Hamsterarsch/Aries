#include <d3d12.h>
#include "dx12/DX12Factory.h"
#include "Error.h"
#include "dx12/DX12Fence.h"


FDX12Fence::FDX12Fence(FDX12Factory &Factory, size_t InitialValue)
{
	ARI_THROW_IF_FAILED(
		Factory.GetDevice()->CreateFence(InitialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence) ),
	"Could not create dx12 fence"
	);


}

EAPITypes FDX12Fence::GetAPIType() const noexcept
{
	return EAPITypes::DX12;

}

size_t FDX12Fence::GetValue()
{
	return m_pFence->GetCompletedValue();

	
}

void FDX12Fence::Signal(size_t Value)
{
	ARI_THROW_IF_FAILED(
		m_pFence->Signal(Value),
	"Unable to signal dx12 fence"
	);


}

void FDX12Fence::Increment()
{
	ARI_THROW_IF_FAILED(
		m_pFence->Signal(m_pFence->GetCompletedValue() + 1),
		"Unable to signal dx12 fence"
	);


}

void FDX12Fence::Decrement()
{
	ARI_THROW_IF_FAILED(
		m_pFence->Signal(m_pFence->GetCompletedValue() - 1),
		"Unable to signal dx12 fence"
	);
	

}

void FDX12Fence::SetEvent(size_t TriggerValue, void *pEvent)
{
	ARI_THROW_IF_FAILED(
		m_pFence->SetEventOnCompletion(TriggerValue, pEvent),
		"Unable to set dx12 fence event"
	);


}

ComPtr<ID3D12Fence> FDX12Fence::GetAPIHandle() const noexcept
{
	return m_pFence;


}
