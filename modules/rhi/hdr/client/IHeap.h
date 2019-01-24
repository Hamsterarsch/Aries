#pragma once


enum class EHeapTypes : unsigned char
{
	Default,
	Upload,
	Readback

};

enum class EResourceCategory : unsigned char
{
	Buffer = 1,
	Texture,
	RT_DS_Texture,
	All

};


class IHeap
{
public:
	virtual ~IHeap() = default;

	virtual EAPITypes GetAPIType() const noexcept = 0;
	virtual size_t GetSizeInBytes() const noexcept = 0;
	virtual std::intptr_t Allocate(size_t SizeInBytes, void *pData) = 0;
	virtual bool HasMSAAAlignment() const noexcept = 0;

};
