#pragma once
#include "dx12/DX12Factory.h"
#include "dx12/DX12CmdAllocator.h"
#include "Error.h"

class FDX12CmdList
{
public:
	FDX12CmdList(FDX12Factory &Factory, const FDX12CmdAllocator &Allocator)
	{
		ARI_THROW_IF_FAILED(
			Factory.GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Allocator.GetAPIHandle().Get(), nullptr, IID_PPV_ARGS(&m_pCmdList)),
		"Could not create direct command list"
		);
		
	}

	ComPtr<ID3D12CommandList> GetAPIHandle() const noexcept { return m_pCmdList; }

protected:

	ComPtr<ID3D12CommandList> m_pCmdList;


};
