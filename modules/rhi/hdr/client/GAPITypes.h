#pragma once
#include "EnumFlags.h"

constexpr unsigned int ARI_DEFAULT_NODE_MASK{ 0 };

enum class EGAPIResourceTypes : char
{
	None,
	SRV,
	CBV,
	UAV


};

enum class EAPITypes : char
{
	DX12


};

enum class EGAPIFormats : int
{
	None
	
};

enum class ESRVDimensions : char
{
	None,
	Buffer,
	Tex1D,
	Tex2D,
	Tex3D,
	TexCube,
	Tex1DArray,
	Tex2DArray,
	TexCubeArray


};

enum class ETexFilterMode : char
{


};

enum class ETexAddressMode : char
{
	Wrap,
	Mirror, 
	Clamp,
	Border,
	MirrorOnce

};

enum class EDescriptorTypes : char
{
	None,
	CBV,
	SRV,
	UAV,
	RTV,
	DSV,
	Sampler

};

enum class EDescriptorHeapTypes : char
{
	CBV_SRV_UAV,
	RTV,
	DSV,
	Sampler

};

enum class EComparisonFunc : char
{
	Never,
	Equal,
	NotEqual,
	Less,
	LessEqual,
	Greater,
	GreaterEqual,
	Always

};

enum class EFDescriptorFlags : int
{
	Default,
	Volatile				= 1 << 0,
	DataVolatile			= 1 << 1,
	DataStatic				= 1 << 2,
	DataStaticAtExecution	= 1 << 3,


};
GENERATE_BINARY_OPERATORS_FOR_ENUM_FLAGS(EFDescriptorFlags);

enum class EFShaderStages : char
{
	VS = 1 << 0,
	PS = 1 << 1,
	GS = 1 << 2,
	TCTRL = 1 << 3,
	TEV = 1 << 4,
	ALL = static_cast<unsigned char>(~0)

};
GENERATE_BINARY_OPERATORS_FOR_ENUM_FLAGS(EFShaderStages);

enum class EShaderStages : char
{
	ALL,
	VS,
	PS,
	GS,
	TCTRL,
	TEV

};

struct FSamplerDesc
{
	ETexFilterMode FilterMode;

	ETexAddressMode U,
		V,
		W;

	unsigned int LowestMipToUse;

	unsigned int HighestMiptoUse;//uint max for all available (?)

	int MipBias;

	unsigned int MaxAnisotropyLevel;


};

/*
enum class EGAPIErrors : int
{
	Ok,
	InvalidArgument,
	OutOfMemory

};
*/