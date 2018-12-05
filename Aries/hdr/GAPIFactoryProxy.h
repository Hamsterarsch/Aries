#pragma once
#include "IGAPIFactory.h"
#include <memory>

class FGAPIFactoryProxy : public IGAPIFactory
{
public:
	static std::shared_ptr<FGAPIFactoryProxy> GetInstance();

	static std::shared_ptr<FGAPIFactoryProxy> GetInstance(EAPIType Type);


	~FGAPIFactoryProxy();

	FGAPIFactoryProxy(FGAPIFactoryProxy &Other) = delete;

	FGAPIFactoryProxy &operator=(FGAPIFactoryProxy &Rhs) = delete;
	
	FGAPIFactoryProxy(FGAPIFactoryProxy &&Other);

	FGAPIFactoryProxy &operator=(FGAPIFactoryProxy &&Rhs);
	
	virtual EAPIType GetApiType() const override;
	
	virtual std::unique_ptr<IWindow> MakeWindow(UINT Width, UINT Height, LPCWSTR pWindowName, WNDPROC pfnWndProc, LPCWSTR pClassName) override;


protected:
	FGAPIFactoryProxy(EAPIType Type);


	static std::shared_ptr<FGAPIFactoryProxy> s_pInstance;

	std::unique_ptr<class IGAPIFactoryInternal> m_pSubject;


};
