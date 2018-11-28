#include "dx11/DX11PipelineState.h"

FDX11PipelineState::FDX11PipelineState(const ComPtr<ID3D11DeviceContext> &pContext) :
	m_pContext{ pContext }
{
}
