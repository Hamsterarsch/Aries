#include "Actor.h"


//Public---------------

FActor::FActor(const std::shared_ptr<IMesh> &pMesh) :
	m_pMesh{ pMesh },
	m_ToWorldMatrix{ DirectX::XMMatrixIdentity() }
{
}

const DirectX::XMMATRIX &FActor::GetToWorldMatrix() const
{
	return m_ToWorldMatrix;


}

const IMesh *FActor::GetMesh() const
{
	return m_pMesh.get();


}

void FActor::SetMesh(const std::shared_ptr<IMesh> &pMesh) noexcept(std::is_nothrow_copy_assignable_v<decltype(pMesh)>)
{	
	m_pMesh = pMesh;


}

std::shared_ptr<IMesh> FActor::GetMeshPtr() const
{
	return m_pMesh;


}

void FActor::Translate(const DirectX::XMVECTOR &Vector)
{
	m_ToWorldMatrix = DirectX::XMMatrixTranslationFromVector(Vector) * m_ToWorldMatrix;
	

}

void FActor::Scale(float X, float Y, float Z) 
{
	m_ToWorldMatrix = DirectX::XMMatrixScaling(X, Y, Z) * m_ToWorldMatrix;


}

void FActor::Rotate(float Pitch, float Yaw, float Roll)
{
	m_ToWorldMatrix = DirectX::XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll) * m_ToWorldMatrix;

}

