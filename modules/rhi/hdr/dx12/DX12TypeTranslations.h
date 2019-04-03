#pragma once
#include "client/GAPITypes.h"
#include <Error.h>
#include <d3d12.h>

DXGI_FORMAT DX12TranslateFormat(EGAPIFormats Format);

D3D12_FILTER DX12TranslateFilterMode(ETexFilterMode Value);

D3D12_TEXTURE_ADDRESS_MODE DX12TranslateAddressMode(ETexAddressMode Mode);

D3D12_DESCRIPTOR_HEAP_TYPE DX12TranslateHeapType(EDescriptorHeapTypes Type);

D3D12_SRV_DIMENSION DX12TranslateSRVDimension(ESRVDimensions Dimension);

D3D12_COMPARISON_FUNC DX12TranslateComparisonFunc(EComparisonFunc Func);

D3D12_DESCRIPTOR_RANGE_FLAGS DX12TranslateDescriptorFlags(EFDescriptorFlags Flags);

D3D12_DESCRIPTOR_RANGE_TYPE DX12TranslateDescriptorTypes(EDescriptorTypes Type);

D3D12_SHADER_VISIBILITY DX12TranslateStagesToVisibility(EShaderStages Stages);
