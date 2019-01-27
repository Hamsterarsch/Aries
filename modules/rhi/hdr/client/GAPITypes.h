#pragma once

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

/*
enum class EGAPIErrors : int
{
	Ok,
	InvalidArgument,
	OutOfMemory

};
*/