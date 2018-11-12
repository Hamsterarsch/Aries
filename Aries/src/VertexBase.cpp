#include "VertexBase.h"


bool FVertexBase::Bind(ID3D11DeviceContext &Context)
{
	Context.VSSetShader(GetShader(), nullptr, 0);
	return true;


}