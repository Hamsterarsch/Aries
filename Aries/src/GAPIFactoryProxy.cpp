#include "Error.h"
#include "IGAPIFactoryInternal.h"
#include "dx12/DX12Factory.h"
#include "GAPIFactoryProxy.h"


//Static------------------

std::shared_ptr<FGAPIFactoryProxy> FGAPIFactoryProxy::s_pInstance;

std::shared_ptr<FGAPIFactoryProxy> FGAPIFactoryProxy::GetInstance(const EAPIType Type)
{
	if (!s_pInstance)
	{
		s_pInstance = std::shared_ptr<FGAPIFactoryProxy>(new FGAPIFactoryProxy(Type));

	}

	return s_pInstance;


}

std::shared_ptr<FGAPIFactoryProxy> FGAPIFactoryProxy::GetInstance()
{
	return s_pInstance;


}


//Public-----------------------

FGAPIFactoryProxy::~FGAPIFactoryProxy() = default;

FGAPIFactoryProxy::FGAPIFactoryProxy(FGAPIFactoryProxy &&Other) = default;

FGAPIFactoryProxy &FGAPIFactoryProxy::operator=(FGAPIFactoryProxy &&Rhs) = default;

EAPIType FGAPIFactoryProxy::GetApiType() const
{
	return m_pSubject->GetApiType();


}

std::unique_ptr<IWindow> FGAPIFactoryProxy::MakeWindow(UINT Width, UINT Height, LPCWSTR pWindowName, WNDPROC pfnWndProc, LPCWSTR pClassName)
{
	return m_pSubject->MakeWindow(Width, Height, pWindowName, pfnWndProc, pClassName);

	
}


//Protected--------------------------------

FGAPIFactoryProxy::FGAPIFactoryProxy(const EAPIType Type)
{
	switch (Type)
	{
	case EAPIType::DX12:
		m_pSubject = std::make_unique<FDX12Factory>();
		break;
	default:
		ARI_THROW_ERROR(-1, "GAPIFactoryProxy:: api not implemented");
		return;
	}


}


