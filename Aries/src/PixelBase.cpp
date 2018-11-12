#include "PixelBase.h"

bool FPixelBase::Bind(ID3D11DeviceContext &Context)
{	
	Context.PSSetShader(GetShader(), nullptr, 0);

	return true;
		

}