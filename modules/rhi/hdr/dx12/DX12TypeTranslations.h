#pragma once
#include "client/GAPITypes.h"



DXGI_FORMAT DX12TranslateFormat(EGAPIFormats Format)
{
	return DXGI_FORMAT_UNKNOWN;


}

D3D12_FILTER DX12TranslateFilterMode(ETexFilterMode Value)
{
	return D3D12_FILTER_MIN_MAG_MIP_POINT;


}

D3D12_TEXTURE_ADDRESS_MODE DX12TranslateAddressMode(ETexAddressMode Mode)
{
	switch (Mode)
	{
	case ETexAddressMode::Wrap:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		break;
	case ETexAddressMode::Mirror:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		break;
	case ETexAddressMode::Clamp:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		break;
	case ETexAddressMode::Border:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		break;
	case ETexAddressMode::MirrorOnce:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		break;
	default:
		ARI_THROW_ERROR(-1, "Invalid address mode enumeration dx12");
		break;
	}


}

D3D12_DESCRIPTOR_HEAP_TYPE DX12TranslateHeapType(EDescriptorHeapTypes Type)
{
	switch (Type)
	{
	case EDescriptorHeapTypes::CBV_SRV_UAV:
		return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	case EDescriptorHeapTypes::RTV:
		return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		break;
	case EDescriptorHeapTypes::DSV:
		return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		break;
	case EDescriptorHeapTypes::Sampler:
		return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		break;
	default:
		ARI_THROW_ERROR(-1, "Invalid descriptor heap type enumeration dx12")
		break;
	}


}

D3D12_SRV_DIMENSION DX12TranslateSRVDimension(ESRVDimensions Dimension)
{
	switch(Dimension)
	{
	case ESRVDimensions::Buffer:
		return D3D12_SRV_DIMENSION_BUFFER;
		break;
	case ESRVDimensions::Tex1D:
		return D3D12_SRV_DIMENSION_TEXTURE1D;
		break;
	case ESRVDimensions::Tex2D:
		return D3D12_SRV_DIMENSION_TEXTURE2D;
		break;
	case ESRVDimensions::Tex3D:
		return D3D12_SRV_DIMENSION_TEXTURE3D;
		break;
	case ESRVDimensions::TexCube:
		return D3D12_SRV_DIMENSION_TEXTURECUBE;
		break;
	case ESRVDimensions::Tex1DArray:
		return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
		break;
	case ESRVDimensions::Tex2DArray:
		return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		break;
	case ESRVDimensions::TexCubeArray:
		return D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
		break;
	default:
		ARI_THROW_ERROR(-1, "Invalid srv dimension enumeration dx12");
	}


}

D3D12_COMPARISON_FUNC DX12TranslateComparisonFunc(EComparisonFunc Func)
{
	switch (Func)
	{
	case EComparisonFunc::Never:
		return D3D12_COMPARISON_FUNC_NEVER;
		break;
	case EComparisonFunc::Equal:
		return D3D12_COMPARISON_FUNC_EQUAL;
		break;
	case EComparisonFunc::NotEqual:
		return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		break;
	case EComparisonFunc::Less:
		return D3D12_COMPARISON_FUNC_LESS;
		break;
	case EComparisonFunc::LessEqual:
		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case EComparisonFunc::Greater:
		return D3D12_COMPARISON_FUNC_GREATER;
		break;
	case EComparisonFunc::GreaterEqual:
		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	case EComparisonFunc::Always:
		return D3D12_COMPARISON_FUNC_ALWAYS;
		break;
	default:
		ARI_THROW_ERROR(-1, "Invalid comparison func enumeration dx12");
		break;
	}


}

D3D12_DESCRIPTOR_RANGE_FLAGS DX12TranslateDescriptorFlags(EFDescriptorFlags Flags)
{
	D3D12_DESCRIPTOR_RANGE_FLAGS APIFlags{};

	if (Flags & EFDescriptorFlags::Volatile)
	{
		APIFlags |= D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
	}

	if (Flags & EFDescriptorFlags::DataVolatile)
	{
		APIFlags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;
	}

	if (Flags & EFDescriptorFlags::DataStatic)
	{
		APIFlags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
	}

	if (Flags & EFDescriptorFlags::DataStaticAtExecution)
	{
		APIFlags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
	}
	
	return APIFlags;


}

D3D12_SHADER_VISIBILITY DX12TranslateStagesToVisibility(EFShaderStages Stages)
{
	D3D12_SHADER_VISIBILITY Visibility{};

	if (Stages == EFShaderStages::ALL)
	{
		return D3D12_SHADER_VISIBILITY_ALL;

	}

	if (Stages & EFShaderStages::VS)
	{
		Visibility |= D3D12_SHADER_VISIBILITY_VERTEX;

	}

	if (Stages & EFShaderStages::GS)
	{
		Visibility |= D3D12_SHADER_VISIBILITY_GEOMETRY;

	}

	if (Stages & EFShaderStages::TCTRL)
	{
		Visibility |= D3D12_SHADER_VISIBILITY_HULL;

	}

	if (Stages & EFShaderStages::TEV)
	{
		Visibility |= D3D12_SHADER_VISIBILITY_DOMAIN;

	}

	if (Stages & EFShaderStages::PS)
	{
		Visibility |= D3D12_SHADER_VISIBILITY_PIXEL;

	}
	
	return Visibility;


}