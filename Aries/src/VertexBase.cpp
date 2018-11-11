#include "VertexBase.h"


bool FVertexBase::Bind(ID3D11DeviceContext &Context)
{
	Context.VSSetShader(GetShader(), nullptr, 0);

	bool Ret{ true };
	if (m_BindFunction)
	{
		Ret &= m_BindFunction(Context);

	}
	return BindVertex(Context) && Ret;


}