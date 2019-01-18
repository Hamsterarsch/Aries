#pragma once
#include "client/GAPITypes.h"

class IFence
{
public:
	virtual ~IFence() = default;

	virtual EAPITypes GetAPIType() const noexcept = 0;

	virtual void Signal() = 0;
	virtual void Reset() = 0;

	/*
	//vulkan only supports signaled, unsignaled fences, so dx12 close interface is not feasible
	virtual size_t GetValue() = 0;
	virtual void Signal(size_t Value) = 0;
	virtual void Increment() = 0;
	virtual void Decrement() = 0;
	virtual void SetEvent(size_t TriggerValue, void *pEvent) = 0;
	*/

};
