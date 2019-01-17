#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi.h>
#include "Error.h"
#include "dx12/DX12Factory.h"
#include "dx12/DX12ReservedBuffer.h"


FDX12ReservedBuffer::FDX12ReservedBuffer(FDX12Factory &Factory, EBufferTypes Type, size_t SizeInBytes)
{
	D3D12_RESOURCE_DESC Desc{};
	Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	Desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	Desc.DepthOrArraySize = 1;
	Desc.Height = 1;
	Desc.DepthOrArraySize = 1;
	Desc.MipLevels = 1;
	Desc.Format = DXGI_FORMAT_UNKNOWN;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//Width equals the size in bytes. 
	//Based on d3dx12 helper library implementation.
	Desc.Width = SizeInBytes;
	
	D3D12_RESOURCE_STATES State{};
	switch (Type)
	{
	case EBufferTypes::Constant:
	case EBufferTypes::Vertex:
		State = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		break;
	case EBufferTypes::Index:
		State = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		break;
	default:
		ARI_THROW_ERROR(-1, "Dx12ReservedBuffer, invalid buffer type.");
	}


	auto Hr = Factory.GetDevice()->CreateReservedResource(&Desc, State, nullptr, IID_PPV_ARGS(&m_pResource));
	



}
