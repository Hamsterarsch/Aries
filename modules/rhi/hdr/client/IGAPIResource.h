#pragma once
#include "client/GAPITypes.h"

class IGAPIResource
{
public:
	virtual ~IGAPIResource() = default;

	virtual UINT64 GetGPUAddress() const = 0;
	virtual size_t GetSizeInBytes() const noexcept = 0;
	virtual EAPITypes GetAPIType() const noexcept = 0;


};
