#include <d3dcompiler.h>
#include "ObjMesh.h"
#include "OBJ_Loader.h"
#include "ModulePath.h"
#include "Error.h"
#include "DeviceResources.h"

using VbType = decltype(objl::Loader::LoadedVertices)::value_type;
using IbType = decltype(objl::Loader::LoadedIndices)::value_type;

//Public----------------------

//want to share ownership with this mesh because
FObjMesh::FObjMesh(const char *pObjPath, const FDeviceResources &DeviceResources) 
{
	static_assert(sizeof(IbType) == 4, "Index buffer type does not match index buffer format");

	if (!s_pObjInputLayout)
	{
		ID3DBlob *pBytecode, *pError;
		D3DCompileFromFile
		(
			FModulePath::MakeExeRelative(L"../../shaders/ObjInputVS.hlsl").data(),
			nullptr,
			nullptr,
			"main",
			"vs_5_0",
			0,
			0,
			&pBytecode,
			&pError

		);
		
		auto Hr = DeviceResources.GetDevice()->CreateInputLayout(s_vIeDescs.data(), static_cast<UINT>(s_vIeDescs.size()), pBytecode->GetBufferPointer(), pBytecode->GetBufferSize(), s_pObjInputLayout.GetAddressOf());
		if (FAILED(Hr))
		{
			throw(FError{ Hr, "Could not create input layout.", __FILE__, __LINE__ });

		}

	}

	objl::Loader Loader{};
	Loader.LoadFile(pObjPath);

	m_IbNum = Loader.LoadedIndices.size();
	m_VbNum = Loader.LoadedVertices.size();
	
	if (Loader.LoadedVertices.size() == 0)
	{
		throw(FError{ -1, "Could not load mesh data from path.", __FILE__, __LINE__});

	}

	//Invert winding order
	for (size_t Index{}; Index < m_IbNum; Index += 3)
	{
		auto Elem1 = Loader.LoadedIndices.at(Index);
		Loader.LoadedIndices.at(Index) = Loader.LoadedIndices.at(Index + 2);
		Loader.LoadedIndices.at(Index + 2) = Elem1;
				
	}

	CD3D11_BUFFER_DESC VbDesc(static_cast<UINT>(m_VbNum * sizeof(VbType)), D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA VbData{};
	VbData.pSysMem = Loader.LoadedVertices.data();
		
	auto VbHr = DeviceResources.GetDevice()->CreateBuffer(&VbDesc, &VbData, &m_pVb);

	CD3D11_BUFFER_DESC IbDesc(static_cast<UINT>(m_IbNum * sizeof(IbType)), D3D11_BIND_INDEX_BUFFER);

	D3D11_SUBRESOURCE_DATA IbData{};
	IbData.pSysMem = Loader.LoadedIndices.data();

	auto IbHr = DeviceResources.GetDevice()->CreateBuffer(&IbDesc, &IbData, &m_pIb);
	
	if (FAILED(VbHr) || FAILED(IbHr))
	{
		throw(FError{ -1, "Failed to create vertex/index buffers.", __FILE__, __LINE__ });

	}
	

}



ID3D11Buffer *const *FObjMesh::GetVb() const
{
	return m_pVb.GetAddressOf();


}

size_t FObjMesh::GetVbSize() const
{
	return m_VbNum;

}

size_t FObjMesh::GetVbMemsize() const
{
	return sizeof(VbType) * GetVbSize();


}

ID3D11Buffer *FObjMesh::GetIb() const
{
	return m_pIb.Get();


}

size_t FObjMesh::GetIbSize() const
{
	return m_IbNum;


}

size_t FObjMesh::GetIbMemsize() const
{
	return sizeof(IbType) * GetIbSize();


}
		
DXGI_FORMAT FObjMesh::GetIbFormat() const
{
	return s_IbFormat;
	

}

const std::vector<D3D11_INPUT_ELEMENT_DESC> &FObjMesh::GetVbLayout() const
{
	return s_vIeDescs;


}

D3D_PRIMITIVE_TOPOLOGY FObjMesh::GetTopologyType() const
{
	return s_TopologyType;


}

bool FObjMesh::Bind(ID3D11DeviceContext &Context) const
{	
	Context.IASetIndexBuffer(GetIb(), GetIbFormat(), 0);

	auto Stride = static_cast<UINT>(sizeof(VbType));
	UINT Offset{};
	Context.IASetVertexBuffers(0, 1, GetVb(), &Stride, &Offset);

	Context.IASetPrimitiveTopology(GetTopologyType());

	Context.IASetInputLayout(s_pObjInputLayout.Get());//todo: Handle different input layouts

	return true;


}

//Protected---------------
const std::vector<D3D11_INPUT_ELEMENT_DESC> FObjMesh::s_vIeDescs =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}

};

ComPtr<ID3D11InputLayout> FObjMesh::s_pObjInputLayout;

//todo: pitfall ?
constexpr D3D_PRIMITIVE_TOPOLOGY FObjMesh::s_TopologyType;
constexpr DXGI_FORMAT FObjMesh::s_IbFormat;