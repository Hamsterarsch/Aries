#pragma once
#include "dx12/DX12Factory.h"
#include "Error.h"

class FDX12CmdAllocator
{
public:
	FDX12CmdAllocator(FDX12Factory &Factory, bool bForBundleRecording)
	{
		D3D12_COMMAND_LIST_TYPE Type{ bForBundleRecording ? D3D12_COMMAND_LIST_TYPE_BUNDLE : D3D12_COMMAND_LIST_TYPE_DIRECT };
		ARI_THROW_IF_FAILED(
			Factory.GetDevice()->CreateCommandAllocator(Type, IID_PPV_ARGS(&m_pCmdAllocator)),
		"Could not create dx12 command allocator"
		);

		
	}

	ComPtr<ID3D12CommandAllocator> GetAPIHandle() const noexcept { return m_pCmdAllocator; }

protected:
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator;

};
