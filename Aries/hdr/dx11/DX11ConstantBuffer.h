#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Error.h"

using Microsoft::WRL::ComPtr;

template <class DataLayout>
class FDX11ConstantBuffer
{
public:
	FDX11ConstantBuffer() = default;
	FDX11ConstantBuffer(ID3D11Device &Device, DataLayout *pInitialData = nullptr);

	bool Initialize(ID3D11Device &Device, DataLayout *pInitialData = nullptr) noexcept;
	DataLayout &Data();
	void Update(ID3D11DeviceContext &Context);
	ID3D11Buffer *Get() const { return m_pBuffer.Get(); }
	ID3D11Buffer *const *GetAddressOf() const noexcept { return m_pBuffer.GetAddressOf(); }
	ComPtr<ID3D11Buffer> GetCom() const { return m_pBuffer; }

protected:
	DataLayout m_Data;

	ComPtr<ID3D11Buffer> m_pBuffer;



};


template<class DataLayout>
FDX11ConstantBuffer<DataLayout>::FDX11ConstantBuffer(ID3D11Device &Device, DataLayout *pInitialData)
{
	if (Initialize(Device, pInitialData))
	{
		throw(FError{ -1, "Failed to create dx11 constant buffer", __FILE__, __LINE__ });

	}


}

template<class DataLayout>
bool FDX11ConstantBuffer<DataLayout>::Initialize(ID3D11Device &Device, DataLayout *pInitialData) noexcept
{
	static_assert(sizeof(DataLayout) % 16 == 0, "Constant buffer data must be 16 byte aligned.");

	CD3D11_BUFFER_DESC Desc{ sizeof(DataLayout), D3D11_BIND_CONSTANT_BUFFER };

	D3D11_SUBRESOURCE_DATA *pSubresource{ nullptr };
	D3D11_SUBRESOURCE_DATA InitData{};
	if (pInitialData)
	{
		InitData.pSysMem = pInitialData;
		pSubresource = &InitData;
		m_Data = *pInitialData;

	}

	auto Hr = Device.CreateBuffer(&Desc, pSubresource, m_pBuffer.GetAddressOf());

	return SUCCEEDED(Hr);


}

template<class DataLayout>
void FDX11ConstantBuffer<DataLayout>::Update(ID3D11DeviceContext &Context)
{
	Context.UpdateSubresource(m_pBuffer.Get(), 0, nullptr, &m_Data, 0, 0);


}

template<class DataLayout>
DataLayout &FDX11ConstantBuffer<DataLayout>::Data()
{
	return m_Data;


}