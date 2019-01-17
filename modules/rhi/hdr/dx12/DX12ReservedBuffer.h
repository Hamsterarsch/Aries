#pragma once
#include "client/IReservedBuffer.h"


using Microsoft::WRL::ComPtr;

class FDX12ReservedBuffer : public IReservedBuffer
{
public:
	FDX12ReservedBuffer(FDX12Factory &Factory, EBufferTypes Type, size_t SizeInBytes);
	
	virtual EAPITypes GetAPIType() const noexcept override { return EAPITypes::DX12; }


protected:
	void *pData;
	ComPtr<ID3D12Resource> m_pResource;


};
