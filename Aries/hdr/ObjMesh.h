#pragma once
#include <wrl/client.h>
#include "IMesh.h"


template <class T> 
class FShaderBase;

using Microsoft::WRL::ComPtr;


class FObjMesh : public IMesh
{
public:
	FObjMesh(const char *pObjPath, const class FDeviceResources &DeviceResources);
	FObjMesh(FObjMesh &Other) = delete;
	FObjMesh(FObjMesh &&Other) = default;

	FObjMesh &operator=(FObjMesh &Rhs) = delete;
	FObjMesh &operator=(FObjMesh &&Rhs) = default;

	
	virtual bool Bind(ID3D11DeviceContext &Context) const override;

	virtual ID3D11Buffer *const *GetVb() const override;
	virtual size_t GetVbSize() const override;
	virtual size_t GetVbMemsize() const	override;
												
	virtual ID3D11Buffer *GetIb() const override;
	virtual size_t GetIbSize() const override;
	virtual size_t GetIbMemsize() const	override;

	virtual DXGI_FORMAT GetIbFormat() const override;
	virtual const std::vector<D3D11_INPUT_ELEMENT_DESC> &GetVbLayout() const override;
	virtual D3D_PRIMITIVE_TOPOLOGY GetTopologyType() const override;



protected:
	static ComPtr<ID3D11InputLayout> s_pObjInputLayout;
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> s_vIeDescs;
	static constexpr D3D_PRIMITIVE_TOPOLOGY s_TopologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	static constexpr DXGI_FORMAT s_IbFormat = DXGI_FORMAT_R32_UINT;

	size_t m_IbNum, m_VbNum;

	ComPtr<ID3D11Buffer> m_pVb;
	ComPtr<ID3D11Buffer> m_pIb;


};

