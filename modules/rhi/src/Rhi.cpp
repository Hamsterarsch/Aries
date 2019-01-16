#include "client/Rhi.h"
#include "dx12/DX12Factory.h"


RHI_API std::shared_ptr<class IGAPIFactory> GetGAPIFactory(EAPIType Type)
{
	switch (Type)
	{
	case EAPIType::DX12:
		return FDX12Factory::GetInstance();
	default:
		return nullptr;
	}

}


