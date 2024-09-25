#pragma once

#include "Gen_App/Config/AppConfig.h"
#include <d3d12.h>
#include <DirectXCollision.h>
#include "MDS/Tools/RTransform.h"
#include "App/Data/Meshes/RSRMesh.h"
#include "App/Data/Physic/RSRCollisionType.h"
#include <vector>

namespace RSRush
{

	enum class RSRColliderType : uint8_t
	{
		RENUM_MIN_VAL(RectCollider, 0),
		RENUM_MAX_VAL(SphereCollider, 1)
	};
	RS_DEFINE_ENUM(RSRColliderType);

	/// <summary>
	/// Collider data, can map to 1 register (16 Bytes)
	/// </summary>
	struct alignas(16) RSRCollider
	{
	public:
		RSRColliderType Type;
		union
		{
			DirectX::BoundingOrientedBox RectData;
			DirectX::BoundingSphere SphereData;
		};
	public:
		RSRCollider();
		RSRCollider(RSRColliderType InType);
		RSRCollider(const DirectX::BoundingOrientedBox& InRectData);
		RSRCollider(const DirectX::BoundingSphere& InSphereData);
	public:
		inline void Transform(const DirectX::XMMATRIX& InTransform) { (*this) = TransformCopy(InTransform); }
		RSRCollider TransformCopy(const DirectX::XMMATRIX& InTransform) const;
		bool OverlapWith(const RSRCollider& InOther) const;

	public:
		DirectX::BoundingBox ComputeBoundBox() const;
		void ExtendBoundBox(DirectX::BoundingBox* InOutBoundingBox) const;

#if DEBUG_PHYSIC
	public:
		bool DebugDraw(ID3D12GraphicsCommandList7* InUploadCommandList) const;
		static bool DebugDrawOrientedBox(const DirectX::BoundingOrientedBox& InOrientedBox, ID3D12GraphicsCommandList7* InUploadCommandList);
		static bool DebugDrawSphere(const DirectX::BoundingSphere& InSphere, ID3D12GraphicsCommandList7* InUploadCommandList);
		static bool DebugDrawBox(const DirectX::BoundingBox& InBox, ID3D12GraphicsCommandList7* InUploadCommandList);
#endif
	};

	struct alignas(16) RSRTrsCollider
	{
	public:
		alignas(16) DirectX::XMMATRIX Transform;
		RSRCollider Collider;
	};
};