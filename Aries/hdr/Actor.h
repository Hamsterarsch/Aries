#pragma once
#include "hdr/IActor.h"
#include "hdr/ShaderCluster.h"

class FActor : public IActor
{
public:
	FActor(const std::shared_ptr<IMesh> &pMesh);

	virtual const DirectX::XMMATRIX &GetToWorldMatrix() const override;

	virtual const IMesh *GetMesh() const override;
	virtual void SetMesh(const std::shared_ptr<IMesh> &pMesh) noexcept(std::is_nothrow_copy_assignable_v<decltype(pMesh)>) override;
	virtual std::shared_ptr<IMesh> GetMeshPtr() const override;

	virtual void Translate(const DirectX::XMVECTOR &Vector) override;
	virtual void Scale(float X, float Y, float Z) override;
	virtual void Rotate(float Pitch, float Yaw, float Roll) override;

	virtual std::list<std::shared_ptr<FRenderPass>> &GetRenderPasses() override
	{
		return m_RenderPasses;


	}

	virtual void AppendPass(std::shared_ptr<FRenderPass> pPass) override
	{
		m_RenderPasses.push_back(std::move(pPass));


	}

	virtual void PrependPass(std::shared_ptr<FRenderPass> pPass) override
	{
		m_RenderPasses.push_front(std::move(pPass));


	}


private:
	DirectX::XMMATRIX m_ToWorldMatrix;
	std::shared_ptr<IMesh> m_pMesh;
	std::list<std::shared_ptr<FRenderPass>> m_RenderPasses;


};
