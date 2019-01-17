#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi.h>
#include <algorithm>
#include "dx12/DX12Factory.h"
#include "dx12/DX12ReservedBuffer.h"


FDX12ReservedBuffer::FDX12ReservedBuffer(FDX12Factory &Factory, size_t SizeInBytes)
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

	/*
	ComPtr<IDXGIDevice> pDXGIDevice;
	auto Hr{ Factory.GetDevice()->QueryInterface(IID_PPV_ARGS(&pDXGIDevice)) };

	ComPtr<IDXGIAdapter> pAdapter;
	pDXGIDevice->GetAdapter(pAdapter.GetAddressOf());

	ComPtr<IDXGIAdapter3> pAdapter3;
	pAdapter->QueryInterface(IID_PPV_ARGS(&pAdapter3));

	DXGI_QUERY_VIDEO_MEMORY_INFO Info{};
	pAdapter3->QueryVideoMemoryInfo(AdapterNumber, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &Info);
	*/
	//what does width represent for the resource, for textures it is the pixel width.
	//transferring the pixel as the unit of memory and assuming the format of buffers as standard float that are 32bit/4bytes
	//dividing the buffer size in bytes by 4 gives the required width.
	//but maybe width equals byte size here.

	Desc.Width = std::ceill(SizeInBytes / 4);

	auto Hr = Factory.GetDevice()->CreateReservedResource(&Desc, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr, IID_PPV_ARGS(&m_pResource));
	

}
