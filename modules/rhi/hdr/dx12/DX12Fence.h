#pragma once
#include <wrl/client.h>
#include "client/IFence.h"

using Microsoft::WRL::ComPtr;

class FDX12Fence : public IFence
{
public:
	FDX12Fence(class FDX12Factory &Factory, size_t InitialValue);

	virtual EAPITypes GetAPIType() const noexcept;

	virtual size_t GetValue() override;
	virtual void Signal(size_t Value) override;
	virtual void Increment() override;
	virtual void Decrement() override;
	virtual void SetEvent(size_t TriggerValue, void *pEvent) override;


	ComPtr<class ID3D12Fence> GetAPIHandle() const noexcept;

protected:
	ComPtr<class ID3D12Fence> m_pFence;


};
