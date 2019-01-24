#pragma once

class IGAPIResource
{
public:
	virtual ~IGAPIResource() = default;

	virtual UINT64 GetGPUAddress() const = 0;
	virtual size_t GetSizeInBytes() const = 0;

};
