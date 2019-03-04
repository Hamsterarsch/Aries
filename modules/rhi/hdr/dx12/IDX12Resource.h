#pragma once
#include "client/IGAPIResource.h"

class IDX12Resource : public IGAPIResource
{
public:
	virtual ~IDX12Resource() = default;

	virtual class ComPtr<ID3D12Resource> GetAPIHandle() const noexcept = 0;



};
