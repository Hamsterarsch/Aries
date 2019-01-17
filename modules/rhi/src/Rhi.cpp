#include "client/Rhi.h"
#include "dx12/DX12Factory.h"


RHI_API std::shared_ptr<class IGAPIFactory> GetGAPIFactory(EAPITypes Type)
{
	switch (Type)
	{
	case EAPITypes::DX12:
		return FDX12Factory::GetInstance();
	default:
		return nullptr;
	}

}


