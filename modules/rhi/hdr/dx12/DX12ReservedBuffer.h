#pragma once
#include "client/IReservedBuffer.h"


using Microsoft::WRL::ComPtr;

class FDX12ReservedBuffer : public IReservedBuffer
{
public:
	FDX12ReservedBuffer(FDX12Factory &Factory, size_t SizeInBytes);
	

protected:
	void *pData;
	ComPtr<ID3D12Resource> m_pResource;


};
