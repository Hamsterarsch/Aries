#include "PixelBase.h"

bool FPixelBase::Bind(ID3D11DeviceContext &Context)
{	
	Context.PSSetShader(GetShader(), nullptr, 0);
	
	bool Ret{ true };
	if (m_BindFunction)
	{
		Ret &= m_BindFunction(Context);
	
	}
	return BindPixel(Context) && Ret;
		

}