#pragma once

#include "MDS/Tools/RCoordinate.h"
#include <DirectXMath.h>

namespace mds
{
	struct RTransform
	{
	public:
		DirectX::XMFLOAT3 Position = { 0.f, 0.f,  0.f };
		DirectX::XMFLOAT3 Rotation = { 0.f, 0.f,  0.f };
		DirectX::XMFLOAT3 Scale = { 1.f, 1.f,  1.f };
	public:
		DirectX::XMMATRIX XM_CALLCONV ComputeTRSMatrix() const;
		DirectX::XMMATRIX XM_CALLCONV ComputeTRSMatrix(DirectX::FXMVECTOR InQuaternionRad) const;
	public:
	};
	static constexpr RTransform TRS_IDENTITY =
	{
		.Position = { 0.f, 0.f,  0.f },
		.Rotation = { 0.f, 0.f,  0.f },
		.Scale = { 1.f, 1.f,  1.f }
	};

	struct RTransformMatrix
	{
	private:
		RTransform m_transform;
		DirectX::XMFLOAT4X4 m_model;
	public:
		RTransformMatrix(const RTransform& InTransform);
		RTransformMatrix(const RTransformMatrix& InTransform) = default;
	public:
		const RTransform& GetTransform() const { return m_transform; };
		void SetTransform(const RTransform& InTransform);
		const DirectX::XMFLOAT4X4& GetMatrix() const { return m_model; }


		void SetPositon(const DirectX::XMFLOAT3& InPosition);
		void SetRotation(const DirectX::XMFLOAT3& InRotation);
		void SetScale(const DirectX::XMFLOAT3& InScale);

		inline void XM_CALLCONV SetPositon(DirectX::FXMVECTOR InNewPosition)
		{
			DirectX::XMStoreFloat3(&m_transform.Position, InNewPosition);
			XMStoreFloat4x4(&this->m_model, m_transform.ComputeTRSMatrix());
		}
		inline void XM_CALLCONV SetScale(DirectX::FXMVECTOR InNewScale)
		{
			DirectX::XMStoreFloat3(&m_transform.Scale, InNewScale);
			XMStoreFloat4x4(&this->m_model, m_transform.ComputeTRSMatrix());
		}

		void LookAtPosition(const DirectX::XMFLOAT3& InTarget, const DirectX::XMFLOAT3& InUpVector = { 0.f, 0.f, 1.f });
	};
}