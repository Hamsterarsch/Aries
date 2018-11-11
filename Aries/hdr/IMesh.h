#pragma once
#include "D3D11.h"
#include <vector>

class IMesh
{
public:
	virtual bool Bind(ID3D11DeviceContext &Context) const = 0;

	virtual ID3D11Buffer * const *GetVb() const = 0;
	virtual size_t GetVbSize() const = 0;
	virtual size_t GetVbMemsize() const = 0;
	   
	virtual ID3D11Buffer *GetIb() const = 0;
	virtual size_t GetIbSize() const = 0;
	virtual size_t GetIbMemsize() const = 0;
	
	virtual DXGI_FORMAT GetIbFormat() const = 0;
	virtual const std::vector<D3D11_INPUT_ELEMENT_DESC> &GetVbLayout() const = 0;
	virtual D3D_PRIMITIVE_TOPOLOGY GetTopologyType() const = 0;



};