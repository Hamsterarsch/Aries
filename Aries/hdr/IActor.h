#pragma once
#include <DirectXMath.h>
#include "hdr/IMesh.h"
#include "ShaderCluster.h"
#include "RenderPass.h"
#include <list>
#include <memory>


class IActor
{
public:
	virtual const DirectX::XMMATRIX &GetToWorldMatrix() const = 0;

	virtual const IMesh *GetMesh() const = 0;
	virtual void SetMesh(const std::shared_ptr<IMesh> &pMesh) noexcept(std::is_nothrow_copy_assignable_v<decltype(pMesh)>) = 0;
	virtual std::shared_ptr<IMesh> GetMeshPtr() const = 0;

	virtual void Translate(const DirectX::XMVECTOR &Vector) = 0;
	virtual void Scale(float X, float Y, float Z) = 0;
	virtual void Rotate(float Pitch, float Yaw, float Roll) = 0;

	virtual std::list<std::shared_ptr<FRenderPass>> &GetRenderPasses() = 0;
	virtual void AppendPass(std::shared_ptr<FRenderPass> pPass) = 0;
	virtual void PrependPass(std::shared_ptr<FRenderPass> pPass) = 0;


};