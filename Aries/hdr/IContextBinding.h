#pragma once
#include <d3d11.h>

class IContextBinding
{
public:
	virtual bool Bind(ID3D11DeviceContext &Context) = 0;


};

