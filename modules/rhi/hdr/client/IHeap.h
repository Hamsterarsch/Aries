#pragma once


enum class EHeapType : unsigned char
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

	virtual size_t GetSizeInBytes() const noexcept = 0;


};
